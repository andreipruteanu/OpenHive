/*
Copyright (c) 2014, Andrei Pruteanu
All rights reserved.

Code is an open source version of HiveKit by Hive-Systems created by 
Andrei Pruteanu, Stefan Dulman and Tomasz Jaskiewicz.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of "Open Hive" nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "queue.h"
#include "types.h"
#include "logging.h"

// default constructor
Queue::Queue(void) {
    q.vals  = new uint8_t[MSG_QUEUE_SIZE+1];
    q.start = 0;
    q.stop  = 0;
    q.size  = MSG_QUEUE_SIZE + 1;
    q.esize = sizeof(uint8_t); // the size of an element
}

// constructor with parameters
Queue::Queue(uint32_t elementSize, uint16_t elemCount) {
    q.vals  = new uint8_t[elemCount + 1];
    q.start = 0;
    q.stop  = 0;
	q.size  = sizeof(elementSize) * elemCount + 1;
	q.esize = elementSize;
}

Queue::~Queue(void) {
	// de-allocate the queue elements vector
	delete(q.vals);
}

void Queue::initQueue(void) {
  q.start = 0;
  q.stop = 0;
}

void Queue::clear(void) {
  uint8_t dummy;
  while (popByte(&dummy)) {};
}


uint32_t Queue::length(void) {
  
  int32_t res = q.stop - q.start;
  
  while (res < 0) {
    res = res + q.size;
  }
  
  res = (res % q.size) / q.esize;
  
  return res;
}


uint32_t Queue::lengthInBytes(void) {

  int32_t res = q.stop - q.start;
  
  while (res < 0) {
    res = res + q.size;
  }
  
  res = (res % q.size);
  
  return res;
}


// push a single byte in the queue
uint8_t Queue::pushByte(uint8_t* inByte) 
{
	uint32_t newStopIdx = (q.stop + 1) % q.size;
	
	// avoid overlapping - leave one empty element in the queue
	if (newStopIdx == q.start) {
		return 0;
	}

	q.vals[q.stop] = *inByte;
	q.stop = newStopIdx;
	
	return 1;
}

// return one character from the queue
uint8_t Queue::popByte(uint8_t* val) 
{
  
  if (q.stop == q.start) {
    return 0;
  }
  
  *val = q.vals[q.start];
  q.start = (q.start + 1) % q.size;
  
  return 1;
}


uint8_t Queue::push(uint8_t* bytes) 
{
	uint32_t i;
		
	for (i=0; i < q.esize; i++) {
		if (!pushByte(&bytes[i])) {
			// remove the pushed bytes?
			// ...
			return 0;
    	}
  	}

	return 1;
}

uint8_t Queue::pop(uint8_t* bytes) 
{

	uint32_t i;

	if (length() == 0) {
		return 0;
	}

	for (i=0; i < q.esize; i++) {
		if (!popByte(&bytes[i])) {
			return 0;
		}
	}

	return 1;
}



// return buffer from queue
uint8_t Queue::popLength(uint8_t* val, uint32_t bufferLength) 
{
  
  uint32_t i;
  
  if (length() < bufferLength) {
    return 0;
  }
  
  for (i=0; i < bufferLength; i++) {
    pop(&val[i * q.esize]);
  }
  
  return 1;
}

// skips to a certain byte and pops
void Queue::popUntilByte(uint8_t ch) {
  
	uint8_t temp;

	while (lengthInBytes() > 0) {
		if (q.vals[q.start] != ch) {
			popByte(&temp);
		} else {
			break;
		}
	}
}

uint8_t Queue::peekByte(uint32_t pos) {
	return q.vals[(q.start + pos) % q.size];
}

void Queue::peekElement(uint32_t pos, uint8_t* elementBytes)
{
	uint32_t i;

	for (i=0; i < q.esize; i++) 
	{
		elementBytes[i] = peekByte(pos * q.esize + i);
	}
}
