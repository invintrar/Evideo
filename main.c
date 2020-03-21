/**
 * @author: DzhL
 * @date: 2/14/2020
 */

#include "main.h"
#include <signal.h>


/* En esta parte se edito desde raspberry v2*/

uint8_t bNrf, bTog, bMrx, bInit;
uint16_t sensor;

uint8_t tx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};
uint8_t rx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};

//Data sent or receive Nrf24L01 +
uint8_t txEnv[8];
uint8_t rxRec[8];

uint32_t valueX;
uint32_t valueY;
uint32_t valueZ;

struct tm *ptr;
time_t t;

static uint8_t run;

void interrupcion(void);

//Catch Ctrl C
void intHandler(int dummy){
	Led_SetLow();
	RF24L01_powerDown();
	run = 0;
}

float fnabs(float a){
		if(a<0)
				a=-a;
		return a;
}

int main(){
	float corriente, promedio;
	float voltajeS;
	uint8_t ipr;
	bNrf = 1;
	bTog = 1;
	bInit = 0;
	bMrx = 1;
	run = 1;
	ipr=promedio=0;

	//Setting Port CE and SPI
	RF24L01_init();

	//Setting address nrf and channel 
	RF24L01_setup(tx_addr, rx_addr, 22);

	Led_SetOutput();

	//Setting Interrupt
	wiringPiISR(RF_IRQ , INT_EDGE_FALLING, interrupcion);



	//Catch Ctrl-C
	signal(SIGINT,intHandler);

	while(run){
		if(bTog){
			bTog = 0;
			Led_SetHigh();
			delay(250);
		}else{
			bTog = 1;
			Led_SetLow();
			delay(250);
		}

		// Get time
		t = time(NULL);
		ptr = localtime(&t);

		//Prepare the buffer to send from the data_to_send struct
		txEnv[0] = 0x01;
		txEnv[1] = ptr->tm_sec;
		txEnv[2] = ptr->tm_min;
		txEnv[3] = ptr->tm_hour;
		txEnv[4] = (ptr->tm_wday)+1;
		txEnv[5] = (ptr->tm_mon)+1;
		txEnv[6] = ptr->tm_mday;
		txEnv[7] = ptr->tm_year-100;


		switch(bNrf){
			case 1:
				if(bMrx==1){
					bNrf =0;
					bMrx=0;
					RF24L01_set_mode_RX();
					printf("-----Mode  RX-----\nEsperando  Dato...\n");
				}else{
					bNrf = 0;
				    printf("\nReceive %d\n",rxRec[0]);
					printf("RC:%d:%d:%d\n",rxRec[3],rxRec[2],rxRec[1]);
					sensor=(rxRec[5]<<8) | rxRec[4];
					voltajeS = sensor*3.3/1023;
					corriente = (fnabs(voltajeS - 1.65)/0.0132)+2;
					printf("Consumo:%.2f mA\n",corriente);
				}
				break;
			case 2:
				bNrf = 1;
				bMrx = 1;
				printf("Data Sent\n");
				break;
			case 3:
				bNrf= 0;
				printf("CPU %d:%d:%d\n", txEnv[2], txEnv[1], txEnv[0]);
				sendData(txEnv, sizeof(txEnv));
				break;
			default:
				break;
		}

	}//End while
	return 0;
}//End Main


void interrupcion(){
	// Return 1:Data Sent, 2:RX_DR, 3:MAX_RT
	bNrf = RF24L01_status();

	if(bNrf){
		RF24L01_read_payload(rxRec, sizeof(rxRec));
		RF24L01_clear_interrupts();
		return;
	}

	RF24L01_clear_interrupts();
}

