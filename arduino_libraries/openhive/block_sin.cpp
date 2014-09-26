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

#include "types.h"
#include "logging.h"
#include "block_sin.h"

// constructor
BlockSin::BlockSin(runtimeState_t* runtimeState_, uint16_t blockId_) {
	runtimeState = runtimeState_;
	blockId      = blockId_;
}

// do the actual operation
void BlockSin::out(void) {
    LOG(LOG_SIN, 2,"SIN Gate Execute");

    // retrieve ports and signals pointers from the script datastructure
    float* signals = scriptHandler->getSignals();
    ports_t* ports = scriptHandler->getPorts();

    float in = signals[ports[blockId].in[0]];

    // write values into output signals
    signals[ports[blockId].out[0]] = (float)(sin(in));

    LOG(LOG_SIN, 1,"SIN Gate in=%f, out=%f",in,signals[ports[blockId].out[0]]);
}

// dummy functions needed because of derivation from abstract base class
void BlockSin::in(void) { }
void BlockSin::step(void) { }
void BlockSin::deallocate(void) { }

