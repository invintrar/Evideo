/**
 * @author: DzhL
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    int id;
    id = fork();

    if(id == 0){
        printf("Soy el hijo y mi id:%d \n",getpid());
        sleep(5);
    }else{
        wait(NULL);
        printf("Soy el padre y mi id: %d\n",getpid());
    }
    return 0;
}
