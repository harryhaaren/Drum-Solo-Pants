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

#ifndef OPENAV_ENGINE_BUFFERS_H
#define OPENAV_ENGINE_BUFFERS_H

#include <stdio.h>
#include <cstring>

/** Buffers
 * This class is used to pass audio/MIDI buffers around the engine.
 */
class Buffers
{
  public:
    Buffers( int sampleRate, int frames, int nAudio, int nMidi = 0 ):
      samplerate( sampleRate ),
      nframes   ( frames ),
      numAudio  ( nAudio ),
      numMidi   ( nMidi )
    {
      if( numAudio )
        audio = new float[numAudio];
      if( numMidi  )
        midi  = new char[numMidi];
    }
    
    ~Buffers()
    {
      if( audio )
        delete audio;
      if( midi )
        delete midi;
    }
    
    int samplerate;
    int nframes;
    
    int numAudio;
    int numMidi;
    
    float* audio;
    char*  midi;
};

#endif // OPENAV_ENGINE_BUFFERS_H

