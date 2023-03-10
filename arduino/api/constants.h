/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _WIRING_CONSTANTS_
#define _WIRING_CONSTANTS_

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include <stdint.h>
#include <stdbool.h>

#define LOW				          (0x0)
#define HIGH				        (0x1)

#define INPUT               (1<<0)
#define INPUT_PULLUP        (1<<1)
#define PULLUP              INPUT_PULLUP

#define INPUT_PULLDOWN      (1<<2)
#define PULLDOWN            INPUT_PULLDOWN

#define OUTPUT              (1<<3)
#define OUTPUT_LOW          (1<<4)
#define OUTPUT_HIGH         (1<<5)

#define CHANGE				      (1<<6)
#define FALLING				      (1<<7)
#define RISING				      (1<<8)

#define DEFAULT				      (1<<30)
#define EXTERNAL			      (1<<31)

enum BitOrder {
	LSBFIRST = 0,
	MSBFIRST = 1
};

typedef bool boolean;
typedef uint8_t byte;
typedef uint16_t word;
typedef uint8_t pin_size_t;

#define PI				                  3.1415926535897932384626433832795
#define HALF_PI				              1.5707963267948966192313216916398
#define TWO_PI				              6.283185307179586476925286766559
#define DEG_TO_RAD			            0.017453292519943295769236907684886
#define RAD_TO_DEG			            57.295779513082320876798154814105
#define EULER				                2.718281828459045235360287471352

//#define min(a,b)			            ((a)<(b)?(a):(b))
//#define max(a,b)			            ((a)>(b)?(a):(b))
//#include <minmax.h>

#ifndef radians
#define radians(deg)              ((deg)*DEG_TO_RAD)
#endif

#ifndef degrees
#define degrees(rad)              ((rad)*RAD_TO_DEG)
#endif

#ifndef sq
#define sq(x) ((x)*(x))
#endif

#ifdef abs
#undef abs
#endif
#define abs(x)                    ((x)>0?(x):-(x))

#define constrain(amt,low,high)		((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#ifdef round
#define round(x)			            ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#endif

#define radians(deg)			        ((deg)*DEG_TO_RAD)
#define degrees(rad)			        ((rad)*RAD_TO_DEG)
#define sq(x)                     ((x)*(x))

#define lowByte(w)			          ((uint8_t) ((w) & 0xff))
#define highByte(w)			          ((uint8_t) ((w) >> 8))

#define bitRead(value, bit)       (((value) >> (bit)) & 0x01)
#define bitSet(value, bit)        ((value) |= (1UL << (bit)))
#define bitClear(value, bit)      ((value) &= ~(1UL << (bit)))
#define bitToggle(value, bit)     ((value) ^= (1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#ifndef bit
#define bit(b)                    (1UL << (b))
#endif

#define bitRead(value, bit)		    (((value) >> (bit)) & 0x01)
#define bitSet(value, bit)		    ((value) |= (1UL << (bit)))
#define bitClear(value, bit)		  ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue)	(bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#ifndef _NOP
#define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

/* C++ prototypes */

#endif /* _WIRING_CONSTANTS_ */