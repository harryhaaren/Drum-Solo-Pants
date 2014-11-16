
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <sndfile.h>

using namespace std;

#include "alsa.hxx"

#define NVOICES 32
const int samplerate = 48000;
const int nframes    = 512;
const int nbufs      = 4;

const char* ttyName  = "/dev/ttyACM0";


class Sample
{
  public:
    Sample( int i )
    {
      std::stringstream s;
      s << "wav/"<< i + 1 << ".wav";
      loadSample( s.str() );
    }
    
    Sample( std::string s )
    {
      loadSample( s );
    }
    
    void loadSample( std::string s )
    {
      sample = 0;
      
      SF_INFO info;
      SNDFILE* const sndfile = sf_open( s.c_str(), SFM_READ, &info);
      
      if ( !sndfile )
      {
        printf("Failed to open sample '%s'\n", s.c_str() );
        free(sample);
        return;
      }
      
      // Read data
      frames = info.frames;
      
      if ( info.channels > 1 )
      {
        printf("Warning, sample %s has %i channels\n", s.c_str(), info.channels);
      }
      
      sample = (float*)malloc(sizeof(float) * frames * info.channels );
      
      if (!sample)
      {
        printf("Failed to allocate memory for sample\n");
        return;
      }
      
      sf_seek(sndfile, 0ul, SEEK_SET);
      
      sf_read_float(sndfile, sample, info.frames * info.channels );
      
      sf_close(sndfile);
    }
    
    ~Sample()
    {
      free( sample );
    }
    
    long frames;
    float* sample;
};

Sample* samples[8];

class Voice
{
  public:
    Voice()
    {
      playing = false;
      sampleNum = 0;
      index = 0;
    }
    
    void play( int smpleNum )
    {
      playing = true;
      sampleNum = smpleNum;
      index = 0;
      
      printf("index %li", index );
    }
    
    float process()
    {
      if( playing )
      {
        if ( ! samples[sampleNum] )
        {
          printf("error, sampleNum %i has no sample loaded\n", sampleNum );
        }
        
        if ( index < samples[sampleNum]->frames )
        {
          return samples[sampleNum]->sample[index++];
        }
        else
        {
          playing = false;
        }
      }
      return 0;
    }
    
    bool playing;
    long index;
    int sampleNum;
};

Voice voices[NVOICES];

/// the process callback
int process(  int nframes_not_used,
              float** inputBuffers,
              float** outputBuffers,
              void* userdata)
{
  int ttyFD = *(int*)userdata;
  
  fd_set rfds;
  struct timeval tv;
  int retval;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET( ttyFD, &rfds);

  /* Wait up to five seconds. */
  tv.tv_sec = 0;
  tv.tv_usec = 1;
  
  retval = select( ttyFD + 1, &rfds, NULL, NULL, &tv);
  
  if ( retval )
  {
    char byte[2];
    read( ttyFD, byte, 2 );
    
    //if( byte[0] != 10 )
    for(int b = 0; b < 2; b++ )
    {
      
      
      if ( byte[b] >= 0 + 0x30 && byte[b] < 8 + 0x30 )
      {
        int trig = byte[b] - 0x30;
        printf("HIT trigger %d\n", trig );
        
        for(int i = 0; i < NVOICES; i++)
        {
          if( ! voices[i].playing )
          {
            voices[i].play( trig );
            printf("voice %i trigger %d\n", i, trig );
          
            break;
          }
        }
      }
      else
      {
        if( byte[b] != 10 )
          printf("trigger num %i : out of bounds!\n", byte[b] );
      }
    }
  }
  
  
  /*
  while ( *ttyDevice )
  {
    char byte;
    *ttyDevice >> byte;
    printf("HIT trigger %d\n", byte );
    
    for(int i = 0; i < NVOICES; i++)
    {
      if( ! voices[i].playing )
      {
        voices[i].play( byte );
        printf("voice %i trigger %d\n", i, byte );
        break;
      }
    }
    
  }
  */
  
  for(int i = 0; i < nframes; i++)
  {
    float tmp = 0;
    for(int v = 0; v < NVOICES; v++ )
    {
      tmp += voices[v].process();
    }
    
    outputBuffers[0][i] = tmp;
    outputBuffers[1][i] = tmp;
  }
}


int main()
{
  // read from the RF device
  int fd = open( ttyName, O_RDONLY );
  
  /*
  ifstream ttyDevice( ttyName );
  if (!ttyDevice)
  {
    // Print an error and exit
    cerr << "Uh oh, " << ttyName << "could not be opened for reading!" << endl;
    exit(1);
  }
  */
  
  
  // load the sample files
  for(int i = 0; i < 6; i++ )
  {
    samples[i] = new Sample( i );
  }
  
  samples[6] = new Sample( "wav/footstep1.wav" );
  samples[7] = new Sample( "wav/footstep2.wav" );
  
  for(int i = 0; i < NVOICES; i++)
  {
    if( ! voices[i].playing )
    {
      //voices[i].play( 0 );
      //printf("voice %i trigger %d\n", i, 0 );
      break;
    }
  }
  
  
  // Start the audio engine
  EngineAlsa* engine = new EngineAlsa( process, (void*)&fd, "hw:0", "hw:0", samplerate, nframes, nbufs, 80 );
  engine -> start();
  
  for(;;)
  {}
  
  
  return 0;
  
}
