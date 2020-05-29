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
	// Setting Port CE and SPI and wiringPi
	RF24L01_init();
	// Setting address nrf and channel 
	RF24L01_setup(tx_addr, rx_addr, CHANNEL);
    // Setting led output
	ledSetOutput();
	// Setting Interrupt for NRF2L01+
	wiringPiISR(RF_IRQ , INT_EDGE_FALLING, interrupcion);
	// Interrupt when timer
	signal(SIGALRM, timer_handler);
	// Interrupt when catch Ctrl-C
	signal(SIGINT,intHandler);
	// delete record video
	//strcpy(tmp,"rm video.avi"); 
	//system(tmp);
	//turn On led
	ledOn();

	// Bucle while
	while(run)
	{
		switch(bNrf)
		{
			case 1://Data received
				bNrf = 4;
				RF24L01_read_payload(rxRec, sizeof(rxRec));
				printf("Dato Recibido Opcion: %d\n", rxRec[0]);
				if(rxRec[0] > 0)
					task(rxRec[0]);
                break;
			case 2://Date Sent
				bNrf = 4;
				printf("Dato Enviado\n");
				break;
			case 3://MAX_RT
				printf("Maximo Numero de Retransmisiones\n");
                bNrf = 4;
				break;
            case 4:// Set module NRF24L01+ in mode reception
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
	RF24L01_clear_interrupts();

} // end Interrupt


// Make a take
void task(uint8_t opc)
{
    switch(opc)
        {
            case 1: // Syncronization clock
				// Send data to station base
				syncClock(2);
                break;
            case 2: // Empezamos la captura de video
				displayClock(CLOCK_REALTIME, "CLOCK_REALTIME");
				timer.it_value.tv_sec = 60;
				timer.it_value.tv_usec = 0; // start in 60 sec
				timer.it_interval.tv_sec = 1;
				timer.it_interval.tv_usec = 0; // period = 1 sec
				setitimer(ITIMER_REAL, &timer, NULL);
                break;
            case 3: // Apagamos la aplicacion
                ledOff();
                RF24L01_powerDown();
                run = 0;
                break;
            default:
                break;
        } // end switch
} // end tarea


//Catch Ctrl C
void intHandler(int dummy)
{
	ledOff();
	RF24L01_powerDown();
	run = 0;
	run1 = 0;
} // end catch Ctrl C


// Function for sync_clock and match clock
void syncClock(int times)
{
	unsigned int iTimeSec = 0;
	unsigned int iTimeNse = 0;
	
	printf("Synchronization %d\n", times);

	iTimeSec = (rxRec[4] << 24) |(rxRec[3] <<16) | (rxRec[2] << 8) | rxRec[1] ;
	iTimeNse = (rxRec[8] << 24) |(rxRec[7] <<16) | (rxRec[6] << 8) | rxRec[5] ;

	struct timespec timeSet;

	timeSet.tv_sec = iTimeSec;
	timeSet.tv_nsec = iTimeNse;

	setClock( CLOCK_REALTIME, &timeSet);
	getTime();
	displayClock(CLOCK_REALTIME, "CLOCK_REALTIME");
	printf("Reloj Igualado\n");
	

}


// get time of the raspberry opc:1 Seconds, microseconds
// other case date
void getTime(void)
{
	unsigned int in[2] = {0};
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) 
	{
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}
    in[0] = (int) ts.tv_sec;
    in[1] = (int) ts.tv_nsec;
	// Get Seconds and convert to uint8_t
	txEnv[0] = 0;
	txEnv[1] = in[0];
	txEnv[2] = (in[0] >> 8);
	txEnv[3] = (in[0] >> 16);
	txEnv[4] = (in[0] >> 24);
	// Get microSeconds and convert to uint8_t
	txEnv[5] = in[1];
	txEnv[6] = (in[1] >> 8);
	txEnv[7] = (in[1] >> 16);
	txEnv[8] = (in[1] >> 24);
	txEnv[9] = 0;
	txEnv[10] = 0;
	txEnv[11] = 0;
	txEnv[12] = 0;
} // end getTime

// Set clock of raspberry pi
void setClock(clockid_t clock, struct timespec *ts)
{
    if(clock_settime(clock, ts) == -1 )
    {
        perror("clock_settime");
        exit(EXIT_FAILURE);
    }
}// set clock of raspberry pi

void timer_handler(int sig)
{
	static bool cont = true;
    if(cont)
    {
        printf("Timer_handler: signal=%d\n", sig);
        displayClock(CLOCK_REALTIME, "CLOCK_REALTIME");
		videoCapture();
        cont = !cont;
        
    }else
    {   
		ledToggle();  
    }      
}

void videoCapture(void)
{
	sprintf(tmp, "ffmpeg -i /dev/video0 -t %02d:%02d:%02d -r 24 -metadata title='Estacion Video' video.avi",
	rxRec[3],rxRec[2],rxRec[1]);
    system(tmp);
}

void displayClock(clockid_t clock, char *name)
{    
    if (clock_gettime(clock, &ts) == -1) 
    {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    printf("%-15s: %10ld.%03ld (", name,
    (long) ts.tv_sec, ts.tv_nsec / 1000000);
    long days = ts.tv_sec / SECS_IN_DAY;
    if (days > 0)
        printf("%ld days + ", days);
    printf("%2ldh %2ldm %2lds", (ts.tv_sec % SECS_IN_DAY) / 3600,
    (ts.tv_sec % 3600) / 60, ts.tv_sec % 60);
    printf(")\n");
}


// Led toggle off/on
void ledToggle(void)
{
	// Flag use for blink Led
	static bool bTog = true;
	//Use for blink led
	if(bTog){
		ledOn();
		bTog = !bTog ;
	}else{
		ledOff();
		bTog = !bTog ;
	}
} // end 


/**
 * End File
 */