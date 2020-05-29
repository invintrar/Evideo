/**
 * @author:DzhL
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void){
		int fd[2];
		pid_t pidC;
		char buf[10];
		int num;

		if(pipe(fd)==-1){
				printf("Errror ala abrir pipe");
		}

		pidC = fork();

		switch(pidC){
				case 0: //hijo
                    close(fd[0]);
					write(fd[1], "abcde", 5);
					close(fd[1]);
					exit(0);
					break;
				case -1: // error
                    printf("Error\n");
                    break;
				default: // padre
                    close(fd[1]);
					num = read(fd[0],buf,sizeof(buf)); 
					printf("Padre lee %d bytes:%s\n", num, buf);
					close(fd[0]);
					exit(0);
	       			break;
		}
		return 0;
}
