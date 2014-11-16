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
  
#ifndef OPENAV_AUDIO_BASE_H
#define OPENAV_AUDIO_BASE_H

#include "debug.hxx"

// for performance / time stuff
#include <sys/time.h>

class Buffers;

/// the process callback
typedef int (*ProcessCB)( int nframes,
                          float** inputBuffers,
                          float** outputBuffers,
                          void* userdata);

class AudioBase
{
  public:
    /// Pass in the process() callback
    AudioBase( ProcessCB cb, void* ud = 0 ) :
      processCB( cb ),
      processUserdata( ud )
    {
    }
    
    virtual ~AudioBase()
    {
    }
    
    virtual void start() = 0;
    virtual void stop()  = 0;
    
    virtual int status() = 0;
    
    /// called by derived classes, allowing % CPU load calculations
    void setMaxProcessTime( long time )
    {
      processMaxTime = time;
      OPENAV_P_NOTE("max process time: %li", processMaxTime );
    }
    
    void processStart()
    {
      processStartTime = getTimeInMicroseconds();
    }
    void processFinish()
    {
      //OPENAV_P_NOTE("process() time: %li", getTimeInMicroseconds() - processStartTime );
    }
    
  
  protected:
    ProcessCB processCB;
    void*     processUserdata;
    
    Buffers*  buffers;
    
    /// performance variables
    long processStartTime;
    long processMaxTime;
    
    long getTimeInMicroseconds()
    {
      struct timeval time;
      gettimeofday (&time, 0);
      return (long)time.tv_sec * 1000000 + (long)time.tv_usec;
    }
};

#endif // OPENAV_AUDIO_BASE_H
