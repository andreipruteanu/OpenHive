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

#include "block_gradient.h"
#include "types.h"
#include "Queue.h"
#include "logging.h"

BlockGradient::BlockGradient(runtimeState_t* runtimeState_, uint16_t blockId_) {
  blockId = blockId_;
  runtimeState = runtimeState_;
  LOG(LOG_ALGGRAD, 2,"Initializing Gradient Block");

  // initialize the gradient
  state->amISource   = 0;
  state->maxHopcount = 250;
  
  // allocate the queue itself
  state->q = new Queue(sizeof(hopMsg_t),GRAD_MSG_Q_SIZE);
}

void BlockGradient::in(void) {
    LOG(LOG_ALGGRAD, 2,"Gradient In");

    // retrieve ports and signals pointers from the script datastructure
    float* signals = scriptHandler->getSignals();
    ports_t* ports = scriptHandler->getPorts();

    state->amISource   = (uint8_t)signals[ports[blockId].in[0]];
    state->maxHopcount = (int32_t)signals[ports[blockId].in[1]];
}

void BlockGradient::out(void) {
  LOG(LOG_ALGGRAD, 2,"Gradient Out");
  float* signals = scriptHandler->getSignals();
  signals[(scriptHandler->getPorts())[blockId].out[0]] = (float)state->hopcount;
}

void BlockGradient::step(void) {
    LOG(LOG_ALGGRAD, 2,"Gradient Step");

    if (state->amISource) {
        state->hopcount = 0;
        state->gradStart = runtimeState->tick;
        LOG(LOG_ALGGRAD, 2,"I am source !!!");
    } else {
        uint8_t minNbrGrad = getMinGrad(blockId);
        uint8_t newHopcount = minNbrGrad + 1;
        
        // if new gradient is smaller or equal to MAX_HOPCOUNT, update
        if (newHopcount <= getMaxHopcount(blockId)) {
            state->hopcount = newHopcount;
        } else {
            state->hopcount = getMaxHopcount(blockId);
        }
    }

    // clear (empty) the queue
    (state->q)->clear();

    LOG(LOG_ALGGRAD, 1,"updated hopcount to: %d", state->hopcount);
}

void BlockGradient::deallocate(void) {
  LOG(LOG_ALGGRAD, 2,"De-allocating Gradient Block");

  // de-allocate state queue
  free(state->q);
}

void BlockGradient::gradConsumeNbrStateMsg(uint16_t id, uint8_t* nbrStateArray) {
  LOG(LOG_ALGGRAD, 2,"Gradient: consuming nbr state msg.");

  gradientState_t *nbrState   = (gradientState_t *)nbrStateArray;

  LOG(LOG_ALGGRAD, 2,"Nbr hopcount: %d", nbrState->hopcount);

  uint8_t nbrHopcount = nbrState->hopcount;

    if (nbrHopcount < getMaxHopcount(id)) {
        hopMsg_t nbrHopMsg;
        nbrHopMsg.hopcount = nbrHopcount;
        nbrHopMsg.tick     = runtimeState->tick;

        addNbrGradToQueue(id, nbrHopMsg);
    }
}

void BlockGradient::addNbrGradToQueue(uint16_t id, hopMsg_t nbrGradMsg)
{
  hopMsg_t lastElem;

  Block** blocks = scriptHandler->getBlocks();

  gradientState_t* state = (gradientState_t*) ( (BlockGradient*) (blocks[id]) )->getState();

  LOG(LOG_ALGGRAD, 2,"adding neighbour to Q:");
  LOG(LOG_ALGGRAD, 2,"nbr hopcount=%d, nbr start ticks=%d",nbrGradMsg.hopcount,nbrGradMsg.tick);

  // if cannot do push, first pop the last element
  LOG(LOG_ALGGRAD,2,"before push q size: %d",(state->q)->length());
  while (! (state->q)->push((uint8_t*)(&nbrGradMsg)) )
  {
    LOG(LOG_ALGGRAD,2,"queue length=%d",(state->q)->length());
    if ( (state->q)->pop( (uint8_t*) (&lastElem) ) ) {
      LOG(LOG_ALGGRAD,2,"queue length after pop=%d", (state->q)->length());
      LOG(LOG_ALGGRAD,2,"popped hop=%d, start ticks=%d",lastElem.hopcount,lastElem.tick);
    } else {
      LOG(LOG_ALGGRAD,2,"error popping the element");
    }
  }
  LOG(LOG_ALGGRAD,2,"after push q size: %d",(state->q)->length());
  //printQ(id); 
}

int32_t BlockGradient::getMaxHopcount(uint16_t id) {
  return state->maxHopcount;
}

uint32_t BlockGradient::getMinGrad(uint16_t id)
{
  uint32_t min = getMaxHopcount(id);
  uint32_t i;

  for (i=0; i < (state->q)->length(); i++)
  {
    hopMsg_t peekMsg;
    (state->q)->peekElement(i,(uint8_t*)&peekMsg);

    if (peekMsg.hopcount < min) 
    {
      min = peekMsg.hopcount;
    }
  }
  LOG(LOG_ALGGRAD, 2,"found min grad=%d",min);

  return min;
}

gradientState_t* BlockGradient::getState(void) 
{
  return state;
}

uint16_t BlockGradient::getStateSize(void) 
{
  return sizeof(state);
}
