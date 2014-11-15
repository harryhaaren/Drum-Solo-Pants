
#include <math.h>
#include "alsa.hxx"

const int samplerate = 48000;
const int nframes    = 512;
const int nbufs      = 4;

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
  //memset( outputBuffers[0], 0, sizeof(float)*nframes );
  //memset( outputBuffers[1], 0, sizeof(float)*nframes );
  
  
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
  EngineAlsa* engine = new EngineAlsa( process, 0, "hw:0", "hw:0", samplerate, nframes, nbufs, 80 );
  
  engine -> start();
  
  for(;;)
  {}
  
  
  return 0;
  
}
