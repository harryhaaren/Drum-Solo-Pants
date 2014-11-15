/*
 * Author: Harry van Haaren 2014
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "alsa.hxx"

EngineAlsa::EngineAlsa( ProcessCB processCB,
                void*   processUserdata,
                const char* pDev,
                const char* cDev,
                int sampRt,
                int nframe,
                int nbuffer,
                int rtPriority ) :
  AudioBase( processCB, processUserdata ),
  _status(OPENAV_ERROR),
  _processing(false),
  _printStatus(false),
  playDev( strdup( pDev )),
  captDev( strdup( cDev )),
  sr     ( sampRt ),
  nframes( nframe ),
  nbufs  ( nbuffer ),
  rtPrio( rtPriority )
{
#ifdef BUILD_TESTS
  OPENAV_P_NOTE("ZitaAlsa version: %i.%i", zita_alsa_pcmi_major_version(), zita_alsa_pcmi_minor_version() );
#endif
  if ( !pDev || !cDev )
  {
    OPENAV_P_ERROR("Playback or Capture device invalid");
    _status = OPENAV_ERROR;
    return;
  }
  
  // get current RT scheduling info
  int max = sched_get_priority_max ( SCHED_FIFO );
  if ( rtPrio > max )
  {
    OPENAV_P_NOTE("RTPRIO max %i: audio thread %i : fix RTPRIO permissions");
  }
  
  // calc max process() time
  setMaxProcessTime( nbufs * nframes / float(sr) * 1000 );
  
  // FIXME: *2 for stereo, more channels??
  inBuf  = new float [nframes * 2];
  outBuf = new float [nframes * 2];
  
  OPENAV_P_NOTE("Engine starting audio thread: RtPrio %i", rtPrio );
  _status = OPENAV_OK;
  
  if ( thr_start(SCHED_FIFO, rtPrio, 0x20000) )
  {
    OPENAV_P_ERROR("Can't run in RT mode, please check RTPRIO permissions.");
    _status = OPENAV_ERROR;
    return;
  }
}

EngineAlsa::~EngineAlsa()
{
  stop();
  
  OPENAV_P_NOTE("Waiting on engine shutdown...");
  // give engine ALSA thread a chance to run
  usleep(125000);
  
  if( playDev )
    free( playDev );
  if( captDev )
    free( captDev );
  
  if( inBuf )
    delete[] inBuf;
  if( outBuf )
    delete[] outBuf;
  
  // release ALSA resources, cleans up valgrind output
  snd_config_update_free_global();
  
  OPENAV_P_NOTE("Engine shutdown successful.");
}

void EngineAlsa::thr_main(void)
{
  if( _status != OPENAV_OK )
  {
    OPENAV_P_ERROR("thr_main() called, but have ERROR status!");
    return;
  }
  int i, k;
  
  Alsa_pcmi* pcmi = new Alsa_pcmi( playDev, captDev, /* controlDev */ 0, sr, nframes, nbufs, /* debug */ true);
  
  if ( pcmi->state() )
  {
    OPENAV_P_ERROR("Cannot open the ALSA device");
    delete pcmi;
    pcmi = 0;
    _status = OPENAV_ERROR;
    return;
  }
  
  if ( (pcmi->ncapt() < 2) || (pcmi->nplay() < 2) )
  {
    OPENAV_P_ERROR("Is not a stereo device");
    delete pcmi;
    pcmi = 0;
    _status = OPENAV_ERROR;
    return;
  }
  
#ifdef BUILD_TESTS
  pcmi->printinfo();
#endif // BUILD_TESTS
  
  // pause thread here until we're ready to process
  while ( !_processing )
  {
    usleep( 10000 );
  }
  
  pcmi->pcm_start();
  
  while ( _processing )
  {
    if ( _printStatus )
    {
      pcmi->printinfo();
      _printStatus = false;
    }
    
    k = pcmi->pcm_wait();
    
    // call EngineBase start, for performance monitoring
    processStart();
    
    
    while (k >= nframes)
    {
      // copy input data to buffers
      pcmi->capt_init (nframes);
      pcmi->capt_chan (0, inBuf + 0      , nframes, 1);
      pcmi->capt_chan (1, inBuf + nframes, nframes, 1);
      pcmi->capt_done (nframes);

      // "do the stuff":
      float* ins[] = { inBuf, inBuf + nframes };
      float* out[] = { outBuf, outBuf + nframes };
      
      // zero output buffers
      memset( out[0], 0, nframes * sizeof(float) );
      memset( out[1], 0, nframes * sizeof(float) );
      
      processCB( nframes, ins, out, processUserdata );

      // copy buffer data to outputs
      pcmi->play_init (nframes);
      pcmi->play_chan (0, outBuf + 0      , nframes, 1);
      pcmi->play_chan (1, outBuf + nframes, nframes, 1);
      
      // zero remaining output channels
      for (i = 2; i < pcmi->nplay (); i++)
        pcmi->clear_chan (i, nframes);
      
      pcmi->play_done (nframes);
      k -= nframes;
    }
    
    // call EngineBase finish, for performance monitoring
    processFinish();
    
    
    int pXrun = pcmi->play_xrun();
    if ( pXrun )
    {
      OPENAV_P_WARN("Playback Xrun of %i", pXrun );
    }
    int cXrun = pcmi->capt_xrun();
    if ( cXrun )
    {
      OPENAV_P_WARN("Capture Xrun of %i", cXrun );
    }
    
  }
  pcmi->pcm_stop();
  OPENAV_P_NOTE("EngineAlsa : playback stopped." );
  
  delete pcmi;
}

char* EngineAlsa::getCardName( int cardNum )
{
  register int  err;
  
  char* cardName;
  
  // Start with first card
  snd_ctl_t *cardHandle;

  // Get next sound card's card number. When "cardNum" == -1, then ALSA
  // fetches the first card
  if ((err = snd_card_next(&cardNum)) < 0)
  {
     printf("Can't get the next card number: %s\n", snd_strerror(err));
     return 0;
  }

  // No more cards? ALSA sets "cardNum" to -1 if so
  if (cardNum < 0)
    return 0;
  
  // Open this card's control interface. We specify only the card number -- not
  // any device nor sub-device too
  {
    char   str[64];
    sprintf(str, "hw:%i", cardNum);
    if ((err = snd_ctl_open(&cardHandle, str, 0)) < 0)
    {
       printf("Can't open card %i: %s\n", cardNum, snd_strerror(err));
       return 0;
    }
  }
  
  {
    snd_ctl_card_info_t *cardInfo;
    
    // We need to get a snd_ctl_card_info_t. Just alloc it on the stack
    snd_ctl_card_info_alloca(&cardInfo);
    
    // Tell ALSA to fill in our snd_ctl_card_info_t with info about this card
    if ((err = snd_ctl_card_info(cardHandle, cardInfo)) < 0)
       printf("Can't get info for card %i: %s\n", cardNum, snd_strerror(err));
    else
    {
      printf("Card %i = %s\n", cardNum, snd_ctl_card_info_get_name(cardInfo));
      cardName = strdup( snd_ctl_card_info_get_name(cardInfo) );
    }
  }

  // Close the card's control interface after we're done with it
  snd_ctl_close(cardHandle);
   
  return cardName;
}
