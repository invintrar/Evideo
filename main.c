/**
 * @author: DzhL
 * @date: 2/14/2020
 */

#include "main.h"

/**
 * Bucle main
 */
int main(int argc, char *argv[])
{
	//Init flag for nrf start in mode transmition
	bNrf = 4;
    // Flag use for blink Led
	bTog = 1;
    // Use for first date receive
	bInit = 1;
    // Use for kept in bucle while main
	run = 1;

	//Setting Port CE and SPI
	RF24L01_init();

	//Setting address nrf and channel 
	RF24L01_setup(tx_addr, rx_addr, CHANNEL);

    //Setting Led
	Led_SetOutput();

	//Setting Interrupt
	wiringPiISR(RF_IRQ , INT_EDGE_FALLING, interrupcion);

	//Catch Ctrl-C
	signal(SIGINT,intHandler);

	strcpy(tmp,"rm video.avi"); 
	system(tmp);

	// Bucle infinite
	while(run)
	{
		//Use for blink led
		if(bTog){
			bTog = 0;
			Led_SetHigh();
			delay(100);
		}else{
			bTog = 1;
			Led_SetLow();
			delay(100);
		}

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

		switch(bNrf)
        {
			case 1://Show Data received
				time_us = (rxRec[8] << 8) | rxRec[7];
				//Show data in display
                printf("Estacion Base: %d:%d:%d:%d   %d/%d/%d\n",
                        rxRec[3], rxRec[2], rxRec[1], time_us, rxRec[5], rxRec[4], rxRec[6]);
				bNrf = 4;
                //tareas(tarea);
                break;
			case 2://Date Sent
				bNrf = 0;
				printf("Dato Enviado\n");
                
                RF24L01_set_mode_RX();
				printf("-----Mode  RX-----\nEsperando  Dato...\n");
				break;
			case 3://MAX_RT
                bNrf = 0;
				printf("CPU %d:%d:%d\n", txEnv[3], txEnv[2], txEnv[1]);
				sendData(txEnv, sizeof(txEnv));
				break;
            case 4:
                bNrf = 0;
                RF24L01_set_mode_RX();
				printf("-----Mode  RX-----\nEsperando  Dato...\n");
                break;
			default:
				break;
		} // end switch
	} // End while
	return 0;
} // End Main


/**
 * Interrupt of the nrf when receive data or sent data
 * or error in transmition.
 */
void interrupcion()
{
	// Return 1:RX_DR(Data Received), 2:Data Sent,
    // 3:MAX_RT(Maximun number retransmition in mode Transmisition
	bNrf = RF24L01_status();

	if(bNrf)
	{
		RF24L01_read_payload(rxRec, sizeof(rxRec));
		RF24L01_clear_interrupts();
		return;
	} // end if
	RF24L01_clear_interrupts();
} // end Interrupt


//Catch Ctrl C
void intHandler(int dummy)
{
	Led_SetLow();
	RF24L01_powerDown();
	run = 0;
} // end catch Ctrl C


// Make a take
void tareas(uint8_t tarea)
{
    switch(tarea)
        {
            case 1: // Igualar reloj
				//Show data in display
                printf("Estacion Base: %d:%d:%d   %d/%d/%d\n",
                        rxRec[0],rxRec[1],rxRec[2],rxRec[3], rxRec[4], rxRec[5]);
				// save format in variable temporal
                sprintf( tmp, "date --set=\"20%02d-%02d-%02d %02d:%02d:%02d\"\n",
                        rxRec[5],rxRec[4],rxRec[3],rxRec[0], rxRec[1], rxRec[2]);
				// execute istruction in terminal
                system(tmp);
                // Show data of time in terminal
                printf("CPU %d:%d:%d\n", txEnv[3], txEnv[2], txEnv[1]);
				// Send data to station base
                sendData(txEnv, sizeof(txEnv));
                break;
            case 2: // Empezamos la captura de video
                sprintf(tmp, "ffmpeg -i /dev/video0 -t %02d:%02d:%02d -r 24 -metadata title='Estacion Video' video.avi",rxRec[3],rxRec[2],0);
                system(tmp);
                break;
            case 3: // Apagamos la aplicacion
                Led_SetLow();
                RF24L01_powerDown();
                run = 0;
                break;
            default:
                break;
        } // end switch
} // end tarea

/**
 * End File
 */
