/*
Copyright (c) 2014, Andrei Pruteanu
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the {organization} nor the names of its
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


#ifndef __QUEUE_H
#define __QUEUE_H

#include <Arduino.h>

/** 
 * \struct the queue structure
**/
typedef struct struct_queue_t {
  uint32_t start;     /**< start pointer       */
  uint32_t stop;      /**< stop pointer        */
  uint32_t size;      /**< size of the queue   */
  uint32_t esize;     /**< size of one element */
  uint8_t *vals;      /**< buffer with values  */
} queue_t;

class Queue {
private:
	queue_t q;

public:

	/** 
 	* Default constructor
	**/
	Queue(void);

	/** 
 	* Constructor with parameters
	**/
	Queue(uint32_t elementSize, uint16_t elemCount);

	/** 
 	* Destructor
	**/
	~Queue(void);

	/** 
 	* Function that initialises the queue variables. 
 	* always call this function first before using the queue structure
	**/
	void initQueue(void);

	/** Function empties the queue.
	**/
	void clear(void);
  
	/** 
 	* Returns the number of elements in the queue
 	* return the queue size: multiple of single element size
	**/
	uint32_t length(void);

	/** 
 	* returns the number of bytes in the queue
 	* return the queue size in bytes
	**/
	uint32_t lengthInBytes(void);

	/** 
 	* push an element into the queue - pay attention, it's a pointer!
 	* @param the queue structure
 	* @param the bytes that are pushed into the queue
 	* @return successful or not
	**/
	uint8_t push(uint8_t* bytes);

	/** 
 	* pop an element from the queue
 	* @param the bytes that are poped from the queue
 	* @return successful or not
	**/
	uint8_t pop(uint8_t *bytes);

	/** 
 	* pops a number of ELEMENTS (not bytes!!!) from the queue
 	* @param the bytes that are poped from the queue
 	* @param the number of elements that are poped
 	* @return successful or not
	**/
	uint8_t popLength(uint8_t *val, uint32_t length);

	/** 
 	* pops BYTES from the queue until a given byte is encountered
 	* @param the stop char
 	* @return successful or not
	**/
	void popUntilByte(uint8_t ch);

	/** 
 	* peeks at the next BYTE in the queue
 	* @param position of the byte in the queue
 	* @return successful or not
	**/
	uint8_t peekByte(uint32_t pos);

	/** 
 	* peeks at the next ELEMENT in the queue
 	* @param position of the start byte in the queue
 	* @return successful or not
	**/
	void peekElement(uint32_t, uint8_t*);

	/** 
 	* helper function that pushes one byte to the queue. 
 	* Do not use these functions from outside.
 	* param the queue in which the byte is pushed
 	* param the pushed byte
	**/
	uint8_t pushByte(uint8_t *);

	/** 
 	* helper function that pops one byte to the queue. 
 	* Do not use these functions from outside.
 	* param the queue in which the byte is poped
 	* param the poped byte 
	**/
	uint8_t popByte(uint8_t *);

};

#endif
