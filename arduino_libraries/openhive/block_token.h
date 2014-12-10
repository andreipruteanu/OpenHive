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

#ifndef __block_token_h
#define __block_token_h

#include "block.h"
#include "queue.h"
#include "script.h"

#define UNKNOWN 255

// the input & output signals
typedef struct AlgTokenState {    

    uint32_t slot;          // current synchronization slot

    uint8_t status;         // in current slot, this is the old status
    uint8_t port;       // where did this message come from - possible ports 0,1,2,3

    uint8_t newStatus;      // in current slot, if nothing changes, this is what the node is going to report
    uint8_t newPort;        // port associated with new status
    
    uint8_t nextStatus;     // status received from neighbors who are already one slot in front of us
    uint8_t nextPort;
    
    uint8_t local;
    uint8_t localPort;
    
    uint32_t timer;  // when was the last time we have seen a token? never...
    uint8_t algType;        // by default circular ring
    uint32_t maxSlots;      // maxslots in the syncronization algorithm
    uint32_t maxTimer;   // expiry time
    
    uint8_t execute;        // flag used to update the component
    uint32_t diameter;      // diameter of the network - or duplicate filtering stuff
} AlgTokenState;

class BlockToken : public Block {
private:

	// pointer to the runtime state
	runtimeState_t* runtimeState;

	// script object
	Script* scriptHandler;

    uint32_t slot;          // current synchronization slot

    uint8_t status;         // in current slot, this is the old status
    uint8_t port;       // where did this message come from - possible ports 0,1,2,3

    uint8_t newStatus;      // in current slot, if nothing changes, this is what the node is going to report
    uint8_t newPort;        // port associated with new status
    
    uint8_t nextStatus;     // status received from neighbors who are already one slot in front of us
    uint8_t nextPort;
    
    uint8_t local;
    uint8_t localPort;
    
    uint32_t timer;  // when was the last time we have seen a token? never...
    uint8_t algType;        // by default circular ring
    uint32_t maxSlots;      // maxslots in the syncronization algorithm
    uint32_t maxTimer;   // expiry time
    
    uint8_t execute;        // flag used to update the component
    uint32_t diameter;      // diameter of the network - or duplicate filtering stuff
	uint16_t blockId;
public:
	BlockToken(runtimeState_t* state_, uint16_t blockId_);
	void in(void);         // dummy
	void out(void);        // does the actual computation
	void step(void);       // dummy
	void deallocate(void); // dummy
	void algTokenConsumeNbrStateMsg(uint16_t, uint8_t*, uint8_t, uint8_t);

};

#endif
