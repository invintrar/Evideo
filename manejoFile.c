#include <stdio.h>
#include <stdlib.h>

long toffset;
long tdelay;

int main(int argc, char *argv[])
{
    toffset = 1234567890;
    tdelay = 1876543210;
    FILE *archivo;
    archivo = fopen("DatosSync.txt","at");
		if(archivo == NULL)
		{
			printf("Error al crear el archivo\n");
		}
		else
		{
			fprintf(archivo, "%ld,%ld\n",toffset,tdelay);
			fclose(archivo);
		}
    return 0;
}
