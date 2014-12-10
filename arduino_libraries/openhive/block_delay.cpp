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

#include "types.h"
#include "logging.h"
#include "block_delay.h"

// constructor
BlockDelay::BlockDelay(runtimeState_t* runtimeState_, uint16_t blockId_) {
	runtimeState = runtimeState_;
	blockId      = blockId_;

  LOG(LOG_DELAY, 2, "Allocating Delay Block");

  // retrieve ports and signals pointers from the script datastructure
  float* signals = scriptHandler->getSignals();
  ports_t* ports = scriptHandler->getPorts();

  // read the delay value
  maxsize = signals[ports[blockId].in[1]];

  // allocate the queue 
  q = new Queue(sizeof(float),maxsize + 5);
    
  LOG(LOG_DELAY, 2, "Allocating Delay Block - done");
}

// do the actual operation
void BlockDelay::out(void) {
    LOG(LOG_DELAY, 2,"Delay Out");

    // retrieve ports and signals pointers from the script datastructure
    float* signals = scriptHandler->getSignals();
    ports_t* ports = scriptHandler->getPorts();

    // 
    float retval = 0;

    // trim the queue
    while (q->length() >= maxsize) {
        q->pop( (uint8_t*) &retval);
    }

    // add new element to queue
    q->push((uint8_t*) &signals[ports[blockId].in[0]]);
    
    // update output
    signals[ports[blockId].out[0]] = retval;

    LOG(LOG_DELAY, 2, "Delay out new value: %f", signals[ports[blockId].out[0]]);
}

// dummy functions needed because of derivation from abstract base class
void BlockDelay::in(void) { }
void BlockDelay::step(void) { }

void BlockDelay::deallocate(void) { 
  // de-allocate the queue
  delete q;
}

