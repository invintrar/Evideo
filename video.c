// Video processing example using FFmpeg
// Written by Ted Burke - last updated 12-2-2017
//
 
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
FILE *pipein; 
FILE *pipeout; 
// Video resolution
#define W 960
#define H 720
static uint8_t run;

//Catch Ctrl C
void intHandler(int dummy){
  // Flush and close input and output pipes
  fflush(pipein);
  pclose(pipein);
  fflush(pipeout);
  pclose(pipeout);
	run = 0;
}

// Allocate a buffer to store one frame
unsigned char frame[H][W][3] = {0};
 
void main()
{
    int x, y, count;
    run = 1; 
    // Open an input pipe from ffmpeg and an output pipe to a second instance of ffmpeg
    pipein = popen("ffmpeg -i /dev/video0 prueba.avi","r");
    //Catch Ctrl-C
	  signal(SIGINT,intHandler);
    
    // Process video frames
    while(run)
    {
        // Read a frame from the input pipe into the buffer
        count = fread(frame, 1, H*W*3, pipein);
         
        // If we didn't get a frame of video, we're probably at the end
        if (count != H*W*3) break;
         
        // Process this frame
        for (y=0 ; y<H ; ++y) for (x=0 ; x<W ; ++x)
        {
            // Invert each colour component in every pixel
            frame[y][x][0] = 255 - frame[y][x][0]; // red
            frame[y][x][1] = 255 - frame[y][x][1]; // green
            frame[y][x][2] = 255 - frame[y][x][2]; // blue
        }
         
        // Write this frame to the output pipe
        fwrite(frame, 1, H*W*3, pipein);
    }
     
}
