
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;


#include "alsa.hxx"

const int samplerate = 48000;
const int nframes    = 512;
const int nbufs      = 4;

const char* ttyName  = "/dev/ttyACM0";

float phase = 0.f;

float freq = 440.f;

float samples_per_cycle = samplerate / freq;
float phase_increment = (1.f / samples_per_cycle);


/// the process callback
int process(  int nframes_not_used,
              float** inputBuffers,
              float** outputBuffers,
              void* userdata)
{
  ifstream* ttyDevice = (ifstream*)userdata;
  
  while ( *ttyDevice )
  {
    char byte;
    *ttyDevice >> byte;
    //char* s = (*ttyDevice).getline();
    
    printf("%d\n", byte );

  }
  
  for(int i = 0; i < nframes; i++)
  {
    float tmp = sin( phase * 2 * 3.1415 );
    
    outputBuffers[0][i] = tmp;
    outputBuffers[1][i] = tmp;
    
    phase += phase_increment;
  }
}


int main()
{
  ifstream ttyDevice( ttyName );
  if (!ttyDevice)
  {
    // Print an error and exit
    cerr << "Uh oh, " << ttyName << "could not be opened for reading!" << endl;
    exit(1);
  }
  
  EngineAlsa* engine = new EngineAlsa( process, (void*)&ttyDevice, "hw:0", "hw:0", samplerate, nframes, nbufs, 80 );
  
  engine -> start();
  
  for(;;)
  {}
  
  
  return 0;
  
}
