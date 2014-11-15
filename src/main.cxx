
#include "alsa.hxx"

const int samplerate = 44100;
const int nframes    = 128 ;

/// the process callback
int process(  int nframes_not_used,
              float** inputBuffers,
              float** outputBuffers,
              void* userdata)
{
  memset( outputBuffers[0], 0, sizeof(float)*nframes );
  memset( outputBuffers[1], 0, sizeof(float)*nframes );
}


int main()
{
  EngineAlsa* engine = new EngineAlsa( process, 0, "hw:0", "hw:0", samplerate, nframes, 4, 80 );
  
  engine -> start();
  
  for(;;)
  {}
  
  
  return 0;
  
}
