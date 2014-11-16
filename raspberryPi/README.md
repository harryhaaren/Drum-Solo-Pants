Drum-Solo Pants: RPi code
=========================

The code here is ready to be compiled on the Raspberry Pi. The code reads bytes 
from the Ciseco Radio USB stick, and plays back .wav samples over the Raspberry 
Pi's audio output.

Compiling
---------
Ensure that libsndfile-dev and libasound-dev are installed on your Raspberry Pi.
These libraries are used to load .wav files into memory, and to output audio to
the soundcard.

running `make` in this directory will build the code successfully assuming that
the dependencies are installed.

Usage
-----
Run the binary called `rpiDrumPlayback`, and then use an Arduino to trigger the 
sample playback using the Radio frequency. The program prints some debug "hit 
trigger" messages, these could be commented out for better performance.

Questions
---------
Any questions about this code can be directed to Harry, <harryhaaren@gmail.com>.
