#include "gps.h"

uint8_t initSerial(void)
{
	if((serialPort = serialOpen("/dev/ttyS0", 9600)) < 0)
	{
		fprintf(stderr, "Unable to open serial device: %s\n", 
		strerror(errno));
		return 1;
	}
	else
	{
		return 0;
	}
}

//Función para adquirir los datos del Gps.
void readGps(uint8_t *trama){
	uint8_t i, ch;

	while(1)
	{
		if(serialGetchar(serialPort) ==  13 && serialGetchar(serialPort) == 10 )
		{
			if(serialGetchar(serialPort) == '$' && serialGetchar(serialPort) == 'G' && serialGetchar(serialPort) == 'P' && serialGetchar(serialPort) == 'R' && serialGetchar(serialPort) == 'M' && serialGetchar(serialPort) == 'C' )
			{
				for(i=0;i<60;i++)
				{
					ch = serialGetchar(serialPort);
					trama[i]=ch;
				}
				break;
			}//End while

		}//End if new line

	}//End while

}//End readGPS

//Get Data Gps
dataGps getDataGps(void)
{
	uint8_t trama[60], i, j, pos[12],  intValue, aux2, flag;
	float faux;

	dataGps gps;
	//Inicializamos 
	flag = 0;
	j = 0;
	aux2=0;
	gps.hour = 0;
	gps.minute = 0;
	gps.second = 0;
	gps.gradosLatitud = 0;
	gps.minutosLatitud = 0;
	gps.segundosLatitud = 0;
	gps.latitud = 0;
	gps.gradosLongitud = 0;
	gps.minutosLongitud = 0;
	gps.segundosLongitud = 0;
	gps.longitud = 0;
	gps.day = 0;
	gps.month = 0;
	gps.year = 0;
	intValue = 0;

	for(aux2 = 0 ; aux2<60; aux2++){
		trama[aux2] = 0;
	}
	
	//Obtenemos los datos
	readGps(trama);

	serialFlush(serialPort);

	for(i = 0 ; i<60; i++){
		if(trama[i] == ',')
		{
			pos[j] = i;
			j++;
		}	
	}

	if( trama[ pos[1] + 1 ] ==  'V'){
		printf("Datos no disponibles\n");
		return gps;
	}

	//Get Time
	//Hora
	intValue =(trama[pos[0]+1]-'0')*10 + (trama[pos[0]+2]-'0');
	//Paso a hora local
	if(intValue > 4)
	{
		intValue -= 5;
		flag = 0;
	}
	else
	{
		flag = 1;
		intValue = 24 -(5 - intValue);
	}

	gps.hour = intValue;

	//Minutos
	intValue =(trama[pos[0]+3]-'0')*10 + (trama[pos[0]+4]-'0');
	gps.minute = intValue;

	//Segundos
	intValue =(trama[pos[0]+5]-'0')*10 + (trama[pos[0]+6]-'0');
	gps.second = intValue;


	//Get Latitud
	//Grados
	intValue =(trama[pos[2] + 1] - '0')*10 + (trama[pos[2] + 2]-'0');
	gps.gradosLatitud = intValue;

	//Minutos y segundos
	intValue =(trama[pos[2] + 3] - '0')*10 + (trama[pos[2] + 4]-'0');
	faux = (trama[pos[2] + 6] - '0')*1000 + (trama[pos[2] + 7] - '0')*100 +	(trama[pos[2] + 8] - '0')*10 + (trama[pos[2] + 9] - '0');
	faux = roundf(faux*6/1000);
	aux2 = faux;
	gps.minutosLatitud = intValue;
	gps.segundosLatitud = aux2;

	//Direccion Norte o Sur
	gps.latitud = trama[pos[3] + 1];

	//Get Longitud
	//Grados
	intValue =(trama[pos[4] + 1] - '0')*100 + (trama[pos[4] + 2]-'0')*10 + (trama[pos[4] + 3] - '0');
	gps.gradosLongitud = intValue;

	//Minutos y segundos
	intValue =(trama[pos[4] + 4] - '0')*10 + (trama[pos[4] + 5]-'0');
	faux = (trama[pos[4] + 7] - '0')*1000 + (trama[pos[4] + 8] - '0')*100 +	(trama[pos[4] + 9] - '0')*10 + (trama[pos[4] + 10] - '0');
	faux = roundf(faux*6/1000);
	aux2 = faux;
	gps.minutosLongitud = intValue;
	gps.segundosLongitud = aux2;

	//Direccion Este o Oeste
	gps.longitud = trama[pos[5] + 1];


	//Get Date
	//Day
	intValue = 0;
	intValue = (trama[pos[8] +1]-'0')*10 + (trama[pos[8] + 2]-'0');
	if(flag){
		intValue--;
	}
	gps.day = intValue;

	//month
	intValue = 0;
	intValue = (trama[pos[8] +3]-'0')*10 + (trama[pos[8] + 4]-'0');
	gps.month = intValue;

	//Year
	intValue = 0;
	intValue = (trama[pos[8] +5]-'0')*10 + (trama[pos[8] + 6]-'0');
	gps.year = intValue;


	return gps;

}
