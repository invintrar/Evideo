/**
 * @author:DzhL
 * @date : 2/8/2020
 */

#include <stdio.h>
#include <signal.h>
#include <stdint.h>

FILE *pipe; 

#define W 960
#define H 720
static uint8_t run;

//Catch Ctrl C
void intHandler(int dummy){
  // Flush and close input and output pipes
  fflush(pipe);
  pclose(pipe);
  run = 0;
}

// Allocate a buffer to store one frame
unsigned char frame[H][W][3] = {0};
 
void main()
{
    int x, y, count;
    run = 1; 
    // Open an input pipe from ffmpeg and an output pipe to a second instance of ffmpeg
    pipe = popen("ffmpeg -i /dev/video0 -r 24 -metadata title='Estacion Video' video.avi","w");

    //Catch Ctrl-C
	  signal(SIGINT,intHandler);
    
    // Process video frames
    while(run)
    {
	} 
}
