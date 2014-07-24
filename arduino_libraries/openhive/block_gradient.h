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

#ifndef __block_gradient_h
#define __block_gradient_h

#include "block.h"
#include "queue.h"
#include "script.h"

/**
 * \def gradient msg q size
**/
#define GRAD_MSG_Q_SIZE 10

// the "hop" packet
typedef struct {
	uint8_t hopcount;
	uint32_t tick;
} hopMsg_t;

// the input & output signals
typedef struct {
    Queue* q;  
    uint32_t gradStart;        // time when gradient started
    uint8_t  hopcount;         // output
    uint8_t  amISource;        // input
    uint32_t maxHopcount;      // max hopcount input
} gradientState_t;

class BlockGradient : public Block {
private:
	// pointer to the main state
	mainState_t* mainState;

	gradientState_t* state;
	// script object
	Script* scriptHandler;
	uint16_t blockId;
public:
	BlockGradient(mainState_t*, uint16_t blockId);
	void in(void);
	void out(void);
	void step(void);
	void deallocate(void);

	// custom algorithm function prototypes
	void gradConsumeNbrStateMsg(uint16_t, uint8_t*);
	int32_t getMaxHopcount(uint16_t id);
	uint32_t getMinGrad(uint16_t id);
};

#endif