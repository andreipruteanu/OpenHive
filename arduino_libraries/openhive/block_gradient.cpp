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

#include "block_gradient.h"
#include "types.h"
#include "Queue.h"
#include "logging.h"

BlockGradient::BlockGradient(uint16_t blockId_) {
	blockId = blockId_;
	LOG(LOG_ALGGRAD, 2,"Initializing Gradient Block");

	// initialize the gradient
  	state->amISource   = 0;
  	state->maxHopcount = 250;
	
	// allocate the queue itself
	state->q = new Queue(sizeof(hopMsg_t),GRAD_MSG_Q_SIZE);
}

void BlockGradient::in(void) {
    LOG(LOG_ALGGRAD, 2,"Gradient In");
    state->amISource   = (uint8_t)scriptHandler->getSignals()[(scriptHandler->getPorts())[blockId].in[0]];
    state->maxHopcount = (int32_t)scriptHandler->getSignals()[(scriptHandler->getPorts())[blockId].in[1]];
}

void BlockGradient::out(void) {

}

void BlockGradient::step(void) {

}

void BlockGradient::deallocate(void) {

}

void BlockGradient::gradConsumeNbrStateMsg(uint16_t, uint8_t*) {

}

int32_t BlockGradient::getMaxHopcount(uint16_t id) {

}

