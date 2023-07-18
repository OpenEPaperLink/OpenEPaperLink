/*
  Copyright (c) 2014 Arduino.  All right reserved.

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

#include "RingBuffer.h"
#include <string.h>

RingBuffer::RingBuffer( void )
{
    memset( _aucBuffer, 0, SERIAL_BUFFER_SIZE ) ;
    clear();
}

void RingBuffer::store_char( uint8_t c )
{
  int i = nextIndex(_iHead);

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if ( i != _iTail )
  {
    _aucBuffer[_iHead] = c ;
    _iHead = i ;
  }
}

void RingBuffer::clear()
{
	_iHead = 0;
	_iTail = 0;
}

int RingBuffer::read_char()
{
	if(_iTail == _iHead)
		return -1;

	uint8_t value = _aucBuffer[_iTail];
	_iTail = nextIndex(_iTail);

	return value;
}

int RingBuffer::available()
{
	int delta = _iHead - _iTail;

	if(delta < 0)
		return SERIAL_BUFFER_SIZE + delta;
	else
		return delta;
}

int RingBuffer::peek()
{
	if(_iTail == _iHead)
		return -1;

	return _aucBuffer[_iTail];
}

int RingBuffer::nextIndex(int index)
{
	return (uint32_t)(index + 1) % SERIAL_BUFFER_SIZE;
}

bool RingBuffer::isFull()
{
	return (nextIndex(_iHead) == _iTail);
}
