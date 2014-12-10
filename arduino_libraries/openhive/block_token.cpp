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
#include "block_token.h"

// constructor
BlockToken::BlockToken(runtimeState_t* runtimeState_, uint16_t blockId_) {
	runtimeState = runtimeState_;
	blockId      = blockId_;

  LOG(LOG_DELAY, 2, "Allocating Token Block");
    
  slot         = 0;
  status       = 0;         // in current slot, this is the old status
  port         = UNKNOWN;   // where did this message come from - possible ports 0,1,2,3
    
  newStatus    = 1;         // in current slot, if nothing changes, this is what the node is going to report
  newPort      = UNKNOWN;   // port associated with newStatus
    
  nextStatus   = 0;         // status received from neighbors who are already one slot in front of us
  nextPort     = UNKNOWN;
    
  timer        = 0;         // when was the last time we have seen a token? never...
  maxTimer     = 0;         // expiry time
  algType      = UNKNOWN;   // by default circular ring
  maxSlots     = 0;         // maxslots in the syncronization algorithm
    
  execute      = 0;         // execute code?
  diameter     = 0;         // network diameter

}

// do the actual operation
void BlockToken::out(void) {
    LOG(LOG_TOKEN, 2,"Token Out");

    // retrieve ports and signals pointers from the script datastructure
    float* signals = scriptHandler->getSignals();
    ports_t* ports = scriptHandler->getPorts();

    signals[ports[blockId].out[0]] = (float)(status + local);
    
    LOG(LOG_TOKEN,2,"  out: %f", signals[ports[blockId].out[0]]);
}

// dummy functions needed because of derivation from abstract base class
void BlockToken::in(void) { 
    LOG(LOG_TOKEN,2,"AlgToken In");

    // retrieve ports and signals pointers from the script datastructure
    float* signals = scriptHandler->getSignals();
    ports_t* ports = scriptHandler->getPorts();

    uint8_t lastslot = slot;

    slot         = (uint32_t)signals[ports[blockId].in[0]];
    algType      = (uint8_t )signals[ports[blockId].in[1]];
    maxSlots     = (uint32_t)signals[ports[blockId].in[2]];
    maxTimer     = (uint32_t)signals[ports[blockId].in[3]];
    diameter     = (uint32_t)signals[ports[blockId].in[4]];
    
    // execute code only if something changed
    if (lastslot != slot) {
        execute = 1;
    } else {
        execute = 0;
    }
    LOG(LOG_TOKEN,2,"AlgToken slot: %d, execute: %d", slot, execute);
}
void BlockToken::step(void) { 
    LOG(LOG_TOKEN,2,"AlgToken Step");

    // shall we execute anything?
    if (!execute) {
        return;
    }
    
    // do we have all the data?
    if (maxSlots == 0) {
        return;
    }
    if (slot == UNKNOWN) {
        return;
    }
    if (algType == UNKNOWN) {
        return;
    }
    
    LOG(LOG_TOKEN,2,"  -- entry:");
    LOG(LOG_TOKEN,2,"  status:%d newstat:%d nextstat:%d", status, newStatus, nextStatus);
    LOG(LOG_TOKEN,2,"  port:%d, newPort:%d, nextPort:%d", port, newPort, nextPort);
    LOG(LOG_TOKEN,2,"  local:%d, localPort:%d", local, localPort);
    LOG(LOG_TOKEN,2,"  timer:%ld", timer);
    
    // logic per algorithm
    switch (algType) {
      case 0:   // circular token

        timer = timer + 1;
        
        // did the timer expire?
        if (timer >= maxTimer) {
            
            timer = 0;
            
            status = 1;
            port = UNKNOWN;
            newStatus = 0;
            newPort = UNKNOWN;
            nextStatus = 0;
            nextPort = UNKNOWN;
            local = 0;
            localPort = UNKNOWN;
            
            break;
        }
        
        // main behavior
        if (newStatus) {
            
            if (timer > (diameter/2)) {
                status = 1;
                port = newPort;
                newStatus = 0;
                newPort = UNKNOWN;
                nextStatus = 0;
                nextPort = UNKNOWN;
                local = 0;
                localPort = UNKNOWN;

                timer = 0;
            } else {
                if (rand()%2) {
                    status = 0;
                    port = 0;
                    newStatus = 0;
                    newPort = UNKNOWN;
                    nextStatus = 0;
                    nextPort = UNKNOWN;
                    local = 1;
                    localPort = newPort;
                } else {
                    status = 1;
                    port = newPort;
                    newStatus = 0;
                    newPort = UNKNOWN;
                    nextStatus = 0;
                    nextPort = UNKNOWN;
                    local = 0;
                    localPort = UNKNOWN;

                    timer = 0;                    
                }
            }
        } else {
            if (local) {
                
                if (rand()%2) {
                    status = 1;
                    port = localPort;
                    newStatus = 0;
                    newPort = UNKNOWN;
                    nextStatus = 0;
                    nextPort = UNKNOWN;
                    local = 0;
                    localPort = UNKNOWN;
                    timer = 0;
                } else {
                    status = 0;
                    port = 0;
                    newStatus = 0;
                    newPort = UNKNOWN;
                    nextStatus = 0;
                    nextPort = UNKNOWN;
                }
                
            } else {
                status = 0;
                port = localPort;
                newStatus = 0;
                newPort = UNKNOWN;
                nextStatus = 0;
                nextPort = UNKNOWN;
                local = 0;
                localPort = UNKNOWN;
            }
        }
        break;
      case 1:
        break;
      default:
        break;  
    };
    
    LOG(LOG_TOKEN,2,"  -- exit:");
    LOG(LOG_TOKEN,2,"  status:%d newstat:%d nextstat:%d", status, newStatus, nextStatus);
    LOG(LOG_TOKEN,2,"  port:%d, newPort:%d nextport:%d", port, newPort, nextPort);
    LOG(LOG_TOKEN,2,"  local:%d, localPort:%d", local, localPort);
    LOG(LOG_TOKEN,2,"  lastTimeSeen:%ld", timer);
}

void BlockToken::deallocate(void) { }

void BlockToken::algTokenConsumeNbrStateMsg(uint16_t id, uint8_t* nbrStateArray, uint8_t sndPort, uint8_t rcvPort)
{
    LOG(LOG_TOKEN,2,"AlgToken: consuming nbr state msg.");

    AlgTokenState* nbrState = (AlgTokenState*)nbrStateArray;

    // was this guy triggered?
    if (!nbrState->status) {
        return;
    }
    
    LOG(LOG_TOKEN,2, "  considering message");
    
    switch (algType) {
        case 0:     // circular token
        
            LOG(LOG_TOKEN,2, "  slot:%d, nbrSlot:%d", slot, nbrState->slot);
            LOG(LOG_TOKEN,2, "  sendPort:%d, rcvPort:%d, nbrPort:%d", sndPort, rcvPort, nbrState->port);
        
            if (slot == nbrState->slot) {
                if ((nbrState->port != sndPort) || (nbrState->port == UNKNOWN)) {
                    // we will get and report this token
                    newStatus = 1;
                    newPort = rcvPort;
                    LOG(LOG_TOKEN,2, "    entered if #1");
                }
            }
            if (maxSlots > 0) {
                if (((slot+1) % maxSlots) == nbrState->slot) {
                    if ((nbrState->port != sndPort) || (nbrState->port == UNKNOWN)) {
                        nextStatus = 1;
                        nextPort = rcvPort;
                        LOG(LOG_TOKEN,2, "    entered if #2");
                    }
                }
            }
            break;
        case 1:     // linear slot
            break;
        default:
            break;
    };
}

