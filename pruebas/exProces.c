#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

char tmp[10224];
int ret;
pid_t pid;

void timer_handler(int sig);

int main(int argc, char *argv[]){
    
    signal(SIGCHLD, timer_handler);

    pid = fork();
    if(pid == -1)
        perror("fork");
    /* the childe ... */
    if(!pid){
        sprintf(tmp,"%02d:%02d:%02d",0,0,4);
        char *const args[] = {"ffmpeg" ,"-i", "/dev/video0", "-t", tmp,
        "-r", "24", "-metadata", "title=Estacion Video", "-loglevel", "quiet", "video.avi", NULL};
        ret = execvp ("ffmpeg", args);
        if (ret == -1){
            perror("execvp");
            exit(EXIT_FAILURE);
        }

    }
    sleep(1);
    int cont = 1;
    while(1){
        sleep(1);
        printf("%02d s\n", cont++);
        if(cont>10){
            exit(EXIT_SUCCESS);
            _exit(ret);
        }
    }
    
}

void timer_handler(int sig){
    printf("Proceso Terminado %d\n", sig);
}