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

#ifndef OPENAV_ENGINE_ALSA
#define OPENAV_ENGINE_ALSA

#include <string>

#include "base.hxx"

#include "config.hxx"
#include "debug.hxx"

#include "pxthread.h"
#include "zita-alsa-pcmi.h"

class EngineAlsa : public AudioBase, public Pxthread
{
  public:
    EngineAlsa( ProcessCB processCB,
                void*     processUserdata,
                const char* pDev,
                const char* cDev,
                int sampRt,
                int nframe,
                int nbuffer,
                int rtPriority = 80 );
    
    ~EngineAlsa();
    
    /// call from -1 until a NULL pointer is returned: return char* should be freed.
    static char* getCardName( int cardNum );
    
    void start(){ _processing = true;  }
    void stop() { _processing = false; }
    
    int status()
    {
      _printStatus = true;
      return _status;
    }

  private:
    int _status;
    
    /// flags for the processing thread
    bool _processing;
    bool _printStatus;
    
    /// audio buffers
    float *inBuf;
    float *outBuf;
    
    /// hardware device to use
    char* playDev;
    char* captDev;
    int sr;
    int nframes;
    int nbufs;
    int rtPrio;
    
    // override PxThread func
    void thr_main(void);
};

#endif // OPENAV_ENGINE_ALSA
