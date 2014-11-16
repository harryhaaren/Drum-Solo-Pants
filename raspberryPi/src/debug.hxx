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

#ifndef OPENAV_DEBUG_HXX
#define OPENAV_DEBUG_HXX

#include "config.hxx"

#define NAME "Buska"

#include <stdio.h>
#include <stdarg.h>

/* Example usage
OPENAV_NOTE( "MessageHere" );
OPENAV_WARN( "MessageHere" );
OPENAV_KILL( "MessageHere" );
OPENAV_TEST( "MessageHere" );
*/

enum DEBUG_LEVEL {
  DEBUG_LEVEL_NOTE = 0,
  DEBUG_LEVEL_WARN,
  DEBUG_LEVEL_ERROR,
  DEBUG_LEVEL_TEST,
  DEBUG_LEVEL_BUFFERS,
};

void openav_debug( int warnLevel, const char* name, const char* file, const char* func, int line,
                  const char* format = 0, ... );

#define OPENAV_P_NOTE( format, args... ) openav_debug( DEBUG_LEVEL_NOTE, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )
#define OPENAV_P_WARN( format, args... ) openav_debug( DEBUG_LEVEL_WARN, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )
#define OPENAV_P_ERROR(format, args... ) openav_debug( DEBUG_LEVEL_ERROR,NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )
// only gets printed if #definde BUILD_TESTS
#define OPENAV_P_TEST( format, args... ) openav_debug( DEBUG_LEVEL_TEST, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )
#define OPENAV_P_BUFFERS( format, args... ) openav_debug( DEBUG_LEVEL_BUFFERS, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )

#endif

