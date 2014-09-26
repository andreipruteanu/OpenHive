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
#include "block_scale.h"

// constructor
BlockScale::BlockScale(runtimeState_t* runtimeState_, uint16_t blockId_) {
	runtimeState = runtimeState_;
	blockId      = blockId_;
}

// do the actual operation
void BlockScale::out(void) {
    LOG(LOG_SCALE, 2,"Scale Execute");

    // retrieve ports and signals pointers from the script datastructure
    float* signals = scriptHandler->getSignals();
    ports_t* ports = scriptHandler->getPorts();

    float val     = signals[ports[blockId].in[0]];
    float inLow   = signals[ports[blockId].in[1]];
    float inHigh  = signals[ports[blockId].in[2]];
    float outLow  = signals[ports[blockId].in[3]];
    float outHigh = signals[ports[blockId].in[4]];

    if (val < inLow) val = inLow;
    if (val > inHigh) val = inHigh;

    float out = outLow;
    if (!isEq(abs(inHigh - inLow), 0)) {        
        out = (val - inLow) / (inHigh - inLow) * (outHigh - outLow) + outLow;
    }

    // write values into output signals
    signals[ports[blockId].out[0]] = out;

    LOG(LOG_SCALE, 1,"Scale val=%f inLow=%f inHigh=%f outLow=%f outHigh=%f out=%f",val,inLow,inHigh,outLow,outHigh,out);
}

uint8_t BlockScale::isEq(float in1, float in2) {
    uint8_t retIsEq = 0;
    if (abs(in1 - in2) < 10e-5) {
        retIsEq = 1;
    }
    return retIsEq;
}

// dummy functions needed because of derivation from abstract base class
void BlockScale::in(void) { }
void BlockScale::step(void) { }
void BlockScale::deallocate(void) { }

