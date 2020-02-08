/*
@author:DzL 
*/
#include "main.h"

/* En esta parte se edito desde raspberry v2*/

uint8_t mutex;
uint8_t bandera;
uint8_t tx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};
uint8_t rx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};

typedef struct _data_to_send{
	uint32_t op1;
	uint32_t op2;
}data_to_send;
data_to_send to_send;

typedef struct _data_received{
	uint32_t add;
	uint32_t sub;
	uint32_t mult;
	uint32_t div;
}data_received;
data_received received;

void interrupcion(void);

int main(){
	unsigned char i;
	bandera = 0;

	RF24L01_init();

	// Direccion de Transmision , Direccion de Recepcion, Canal
	RF24L01_setup(tx_addr, rx_addr, 12);

	Led_SetOutput();

	wiringPiISR(RF_IRQ , INT_EDGE_FALLING, interrupcion);

	Led_SetLow();
	to_send.op1 = 1;
	to_send.op2 = 1;

	//Prepare the buffer to send from the data_to_send struct
	uint8_t buffer_to_send[32];

	for (i = 0; i < 32; i++){
		buffer_to_send[i] =0x41;
	}

	/*
	*((data_to_send *) &buffer_to_send) = to_send;
	*/

	while(1){
		mutex = 0;

		/*Set Mode Transmition (TX) */
		RF24L01_set_mode_TX();

		/*Write Payload*/
		RF24L01_write_payload(buffer_to_send, sizeof(buffer_to_send));

		//Wait for the buffer to be sent
		printf("Wait for the buffer to be sent...\n");
		while(!mutex);
		if(mutex != 1){
			printf("The transmission failed\n\n");
			//return 0;
		}
		printf("Transmision Completada.\n\n");

		//Wait for the response
		/** Todo: implement a  timeout if nothing is received after a certain amoun of time*/
		printf("Wait for the response\n");

		mutex = 0;

		/*Set Mode RX*/
		RF24L01_set_mode_RX();

		while(!mutex);

		if(mutex == 1){
			uint8_t recv_data[32];

			RF24L01_read_payload(recv_data, sizeof(recv_data));

			printf("Data Recive:");
			for(i = 0; i < sizeof(recv_data); i++){
				printf("%X",recv_data[i]);
			}
			printf("\n\n");

			received = *((data_received *) &recv_data);

			asm("nop");//Place a breakpoint here to see memory
		}else{
			printf("Something happened\n");
		}

	}
}


void interrupcion(){
	uint8_t sent_info;

	if(bandera == 0)
		Led_SetHigh();
	else
		Led_SetLow();

	bandera = !bandera;

	if((sent_info = RF24L01_was_data_sent())){
		//Packet was sent or max retrie reached
		mutex = sent_info;
		RF24L01_clear_interrupts();
		return;
	}

	if(RF24L01_is_data_available()){
		//Packer was received
		mutex = 1;
		RF24L01_clear_interrupts();
		return;
	}


	RF24L01_clear_interrupts();
}
