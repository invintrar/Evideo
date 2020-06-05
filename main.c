/**
 * @author: DzhL
 * @version: v1.0
 * @since: 2/14/2020
 */

#include "main.h"

/**
 * @brief Bucle main
 */
int main(int argc, char *argv[])
{
	uint8_t i;
	// delete record video if exist 
	if(existFile())
    {
        sprintf(tmp, "rm video.avi");
        system(tmp);
        printf("Archivo video.avi borrado\n");
    }
	// Setting Port CE and SPI and wiringPi
	RF24L01_init();
	// Setting address nrf and channel 
	RF24L01_setup(tx_addr, rx_addr, CHANNEL);
    // Setting led output
	ledSetOutput();
	// Setting Interrupt for NRF2L01+
	wiringPiISR(RF_IRQ , INT_EDGE_FALLING, interrupcionNRF);
	// Interrupt when timer
	signal(SIGALRM, interruptTimer);
	// Interrupt when catch Ctrl-C
	signal(SIGINT, intHandler);
	// Interrupt when end procesos cause fo execvp
	signal(SIGCHLD, interruptProcessEnd);
	//turn On led
	ledOn();
	// set mode RX
	RF24L01_set_mode_RX();
	printf("EVideo Esperando datos EBase\n");
	
	// Bucle while
	while(run)
	{
		switch(bNrf)
		{
			case 1://Data received
				bNrf = 0;
				//printf("Dato  recibido...\n");
				RF24L01_read_payload(rxRec, sizeof(rxRec));
				//printf("rxRec[10] %d\n", rxRec[10]);
				delay(4);
				if(rxRec[0] > 0)
					task(rxRec[0]);
                break;
			case 2://Date Sent
				bNrf = 0;
				//printf("Dato   Enviado...\n");
				RF24L01_set_mode_RX();
				//printf("Esperando Dato...\n");
				break;
			case 3://MAX_RT
				printf("Maximo Numero de Retransmisiones\n");
				RF24L01_set_mode_RX();
				printf("Esperando Dato...\n");
                bNrf = 0;
				break;
			default:
				break;
		} // end switch
	} // End while
	return 0;
} // End Main


/**
 * @brief Interrupt of the nrf when receive data or sent data
 * or error in transmition.
 * 
 */
void interrupcionNRF()
{
	// Return 1:RX_DR(Data Received), 2:Data Sent,
    // 3:MAX_RT(Maximun number retransmition in mode Transmisition
	bNrf = RF24L01_status();
	RF24L01_clear_interrupts();
} // end Interrupt


/**
 * @brief Se usa para realiza tareas: Sincronizar el reloj,
 * Captura del video, detener la aplicacion.
 * 
 * @param opc 
 */
void task(uint8_t opc)
{
    switch(opc)
        {
            case 1: // Syncronization clock
				syncClock();
                break;
            case 2: // Empezamos la captura de video
				displayClock(CLOCK_REALTIME, "CLOCK_REALTIME");
				timer.it_value.tv_sec = 10;// colocar 59
				timer.it_value.tv_usec = 0; // start in 60 sec
				timer.it_interval.tv_sec = 1;
				timer.it_interval.tv_usec = 0; // period = 1 sec
				setitimer(ITIMER_REAL, &timer, NULL);
                break;
            case 3: // Apagamos la aplicacion
				printf("Apagado de la aplicacion remotamente\n");
                ledOff();
                RF24L01_powerDown();
				exit(EXIT_SUCCESS);
				_exit(ret);
                run = 0;
                break;
            default:
                break;
        } // end switch
} // end tarea


/**
 * @brief Interrupcion utilizada para terminar el programa 
 * utilizando Ctrl c
 * 
 * @param dummy 
 */
void intHandler(int dummy)
{
	ledOff();
	RF24L01_powerDown();
	exit(EXIT_SUCCESS);
	_exit(ret);
	run = 0;
} // end catch Ctrl C


/**
 * @brief Funcion para syncronizar los relojes de la estacion base
 * y la estacion video.
 * 
 */
void syncClock()
{
	if (bSync){
		printf("Sincronizacion Iniciada...\n");
		bSync = !bSync;
	}
	
	if(cSync < TIMES)
	{
		if(rxRec[10]== 1 )
		{
			convertCharToInt(t1);
			ms_diff = syncDiffMS(t1);
			//Obtenemos el tiempo y 
			//colocamo los datos en una variable globar txEnv[]
			getTime(t3);
			txEnv[0] = 2;
			sendData(txEnv);
		}
		else if(rxRec[10] == 2)
		{
			convertCharToInt(t1);
			sm_diff = delayDifSM(t1);
			long offset = (ms_diff - sm_diff)/2;
			long delay = (ms_diff + sm_diff)/2;
			//agregamos a la suma ofseet y delay
			sum_offset += offset;
			sum_delay += delay;
			txEnv[0]=1;
			sendData(txEnv);
			cSync++;
		}
	}
	else
	{
		// Show results
		printf("Average Offset = %ld ns\n", sum_offset/(TIMES));
		printf("Average Delay  = %ld ns\n", sum_delay/(TIMES));
		printf("Done!\n");
		printf("Sincronizacion Terminada\n");
		txEnv[0]=3;
		sendData(txEnv);
		// Use for save dates in DatosSync
		archivo = fopen("DatosSync.csv","at");
		if(archivo == NULL)
		{
			printf("Error al crear el archivo\n");
		}
		else
		{
			// Column 1 Offset(ns), Column 2 delay (ns)
			fprintf(archivo, "%ld,%ld\n",sum_offset/(TIMES),sum_delay/(TIMES));
			fclose(archivo);
		}
		
		/*
		//Set clock
		struct timespec timeSet;
		int in[2] = {0};
		getTime(in);
		timeSet.tv_sec = in[0];
		timeSet.tv_nsec = in[1];
		setClock( CLOCK_REALTIME, &timeSet);
		displayClock(CLOCK_REALTIME, "CLOCK_REALTIME");
		printf("Sincronizacion Terminada\n");
		*/
		// init variable
		cSync = 0;
		sum_offset = 0;
		sum_delay = 0;
	}
} // en synClock


/**
 * @brief Obtiene el tiempo del reloj de tiempo real del raspberry pi
 * ademas coloca dicha informacion en el vector txEnv para enviar los 
 * datos.
 * 
 * @param in 
 */
void getTime(int in[2])
{
	if(in != NULL)
	{
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
		// Get nanoSeconds and convert to uint8_t
		txEnv[5] = in[1];
		txEnv[6] = (in[1] >> 8);
		txEnv[7] = (in[1] >> 16);
		txEnv[8] = (in[1] >> 24);
		txEnv[9] = 0;
		txEnv[10] = 0;
		txEnv[11] = 0;
		txEnv[12] = 0;
	}
	
} // end getTime


/**
 * @brief Set the Clock object, configura la hora y fecha
 * 
 * @param clock 
 * @param ts 
 */
void setClock(clockid_t clock, struct timespec *ts)
{
    if(clock_settime(clock, ts) == -1 )
    {
        perror("clock_settime");
        exit(EXIT_FAILURE);
    }
}// set clock of raspberry pi


/**
 * @brief Timer para iniciar la captura luego de 1 min luego de llegar 
 * la instruccion ademas prende y apaga el led cada segundo.
 * 
 * @param sig 
 */
void interruptTimer(int sig)
{
	static bool cont = true;
    if(cont)
    {
        displayClock(CLOCK_REALTIME, "CLOCK_REALTIME");
		videoCapture();
        cont = !cont;
    }else
    {   
		ledToggle();  
    }      
}


/**
 * @brief Captura la webCam generando un nuevo proceso
 *  utililizando fork y execvp
 * 
 */
void videoCapture(void)
{
	pid_t pid;
	printf("Iniciando Captura de video...\n");
	pid = fork();
    if(pid == -1)
        perror("fork");
    /* the childe ... */
    if(!pid){
        sprintf(tmp,"%02d:%02d:%02d",rxRec[3],rxRec[2],rxRec[1]);
        char *const args[] = {"ffmpeg" ,"-i", "/dev/video0", "-t", tmp,
        "-r", "24", "-metadata", "title=Estacion Video", "-loglevel", "quiet", "video.avi", NULL};
        ret = execvp ("ffmpeg", args);
        if (ret == -1){
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
}


/**
 * @brief Muestra la hora del reloj de tiempo real del raspberry pi
 * para lo cual se necesita el identificador del clock y el nombre
 * 
 * @param clock 
 * @param name 
 */
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


/**
 * @brief Funcion utilizado para realizar el cambio de prendido a apagado
 * del led
 */
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
 * @brief Funcion utilizada para verificar si existe un archivo video.avi
 * Si existe returno 1 caso contrario 0.
 * @return uint8_t 
 */
uint8_t existFile(void)
{
    FILE *arch;
    arch = fopen("video.avi","r");
    if (arch == NULL)
    {
        return 0;
    }
    else
    {
        fclose(arch);
        return 1;  
    }
} // en exist


/**
 * @brief Esta funcion se genera cuando termina la gravacion del video 
 * en el tiempo establecido por la estacion base y ademas finaliza el programa
 * automaticament
 * 
 * @param sig 
 */
void interruptProcessEnd(int sig)
{
    printf("Fin de Caputura de Video y Termina el programa\n");
	run = 0;
	exit(EXIT_SUCCESS);
	_exit(ret);
}


/**
 * @brief Calcula la diferencia del tiempo(t1) recibido del estacion base, 
 * master con el tiempo (t2) obtenido en este instante. 
 * 
 * @param t1 
 * @return long 
 */
long syncDiffMS(int t1[2])
{
	int t2[2] = {0};
	getTime(t2);
	return (TO_NSEC(t2) - TO_NSEC(t1));
} // end dms


/**
 * @brief Calcula la diferencia del tiempo(t3) del sclavo y el tiempo(t4) 
 * recibido de la estacion base master.
 * 
 * @param t4 
 * @return long 
 */
long delayDifSM(int t4[2]) 
{
	return (TO_NSEC(t4) - TO_NSEC(t3));
} // end dsm


/**
 * @brief Convierte de un vector de uint8_t de datos recibidos a un valor int.
 * 
 * @param rxRec datos recibidos del modulo NRF24L01+ variable global
 * @return int out[2]
 */
void convertCharToInt(int out[2]){
	int aux = 0;
	aux = (rxRec[4] << 24) |(rxRec[3] <<16) | (rxRec[2] << 8) | rxRec[1];
	out[0] = aux;//seconds
	aux = (rxRec[8] << 24) |(rxRec[7] <<16) | (rxRec[6] << 8) | rxRec[5] ;
	out[1] = aux;//nseconds
} // end cchartoint

/**
 * End File
 */