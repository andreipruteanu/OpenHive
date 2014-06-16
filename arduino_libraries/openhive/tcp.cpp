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

#include "tcp.h"
#include "logging.h"
#include "types.h"

TCP::TCP(mainState_t* _mainState) {
	mainState = _mainState;

	for(uint8_t i = 0; i < SERIAL_PORT_COUNT; i++) {
		queues[i] = new Queue();
	}
}

TCP::~TCP(void) {
	for(uint8_t i = 0; i < SERIAL_PORT_COUNT; i++) {
		// de-allocate the queues
		delete(queues[i]);
	}
}

void TCP::import(void) {
  message_t newMsg;

  for (uint8_t portId = 0; portId < SERIAL_PORT_COUNT+1; portId++) {

    LOG(LOG_TRANSP,4,"importing from port %d started",portId);

    Queue q;
    
    while (q.length() >= PACKET_HEADER_LENGTH) {

        LOG(LOG_TRANSP, 2, "queue %d size: %d", portId, q.length());
        LOG(LOG_TRANSP, 2, "first bytes: -%c-%c-%c-%c-%c-%c-", q.peekByte(0), q.peekByte(1), q.peekByte(2), q.peekByte(3), q.peekByte(4), q.peekByte(5));
        
        q.popUntilByte(HEADER_START_MARKER);

        LOG(LOG_TRANSP, 2, "new queue %d size: %d", portId, q.length());
    
        if (q.length() < PACKET_HEADER_LENGTH) {
            break;
        }

        //LOG(LOG_PRINT,  2, "1");
        
        // test if length is ok
        if (q.peekByte(1) > MSG_LENGTH) {
            // pop headerStartMarker
            q.pop(&newMsg.headerStartMarker);
            continue;
        } 

        //LOG(LOG_PRINT,  2, "2");

        // length of the packet is on position 2       
        if (q.length() >= q.peekByte(1)) {

            //LOG(LOG_PRINT,  2, "3");

            // format a message
            q.pop(&newMsg.headerStartMarker);
            q.pop(&newMsg.length);
            q.pop(&newMsg.type);
            q.pop(&newMsg.port);

            // in case of errors, protect here for type check and port check.

            q.pop(&newMsg.checksum);
            q.pop(&newMsg.scriptVer);
            q.popLength(newMsg.payload, newMsg.length-PACKET_HEADER_LENGTH);

            LOG(LOG_TRANSP, 4, "Transport: headerStartMarker = %d", newMsg.headerStartMarker);
            LOG(LOG_TRANSP, 4, "Transport: length = %d", newMsg.length);
            LOG(LOG_TRANSP, 4, "Transport: type = %d", newMsg.type);
            LOG(LOG_TRANSP, 4, "Transport: port = %d", newMsg.port);
            LOG(LOG_TRANSP, 4, "Transport: checksum = %d", newMsg.checksum);
            LOG(LOG_TRANSP, 4, "Computed checksum = %d", getMsgChecksum(&newMsg));
            //LOG(LOG_TRANSP, 4, "Transport: scriptVer = %d", newMsg.scriptVer);
            
            //LOG(LOG_PRINT,  4, "Transport: scriptVer = %d", newMsg.scriptVer);
            
            LOG(LOG_TRANSP, 4, "Proc msg start. size(q)=%d",q.length());

            // use or discard message?
            if (newMsg.checksum != getMsgChecksum(&newMsg)) {
                LOG(LOG_TRANSP, 1, "Bad checksum (type: %d, chksum=%d, expected=%d)", newMsg.type, newMsg.checksum, getMsgChecksum(&newMsg));
                LOG(LOG_TRANSP, 1, "Msg len=%d"  , newMsg.length);
                LOG(LOG_TRANSP, 1, "Port=%d"     , newMsg.port);
                LOG(LOG_TRANSP, 1, "nbr scriptVer=%d", newMsg.scriptVer);
                LOG(LOG_TRANSP, 1, "own scriptVer=%d", mainState->scriptVer);
                LOG(LOG_TRANSP, 1, "Msg payload = %d %d %d %c%c%c", newMsg.payload[0],newMsg.payload[1],newMsg.payload[2],newMsg.payload[0],newMsg.payload[1],newMsg.payload[2]);
                
            } else {
                if (   (newMsg.scriptVer == mainState->scriptVer)    || 
                       (newMsg.scriptVer == mainState->newScriptVer) || 
                       (newMsg.type == MSG_NEED_SCRIPT)              || 
                       (newMsg.type == MSG_DEALLOC)                  || 
                       (newMsg.type == MSG_REBOOT)                   || 
                       (newMsg.type == MSG_LOGLEVEL)                 || 
                       (newMsg.type == MSG_STATUS)                   || 
                       (newMsg.type == MSG_SEND_SCRIPT_VER)          || 
                       (newMsg.type == MSG_SEND_FIRM_VER)
                   ) {
                    LOG(LOG_TRANSP, 3, "Nbr ver: %d. My ver: %d", newMsg.scriptVer, mainState->scriptVer);
                    processReceivedMessage(&newMsg, portId);
                } else if(isScriptVerLarger(newMsg.scriptVer, mainState->newScriptVer)) {
                    LOG(LOG_TRANSP, 2, "Got new nbr new ver: %d. Mine is: %d",newMsg.scriptVer,mainState->scriptVer);
                    deallocateNewFile();
                    tcpState.askForFile = 1; // ask the new script
                    mainState->newScriptVer = newMsg.scriptVer; // record the new script version
                } else {
                    LOG(LOG_TRANSP, 3, "Ignored old script msg. Ver: %d",newMsg.scriptVer);
                }
            }

            LOG(LOG_TRANSP, 4, "Proc. msg end. size(q)=%d",q.length());

        } else {
            break;
        } 

        //LOG(LOG_TRANSP,1,"q size = %d and is >= HEADER_LENGTH",queueSize(&q));
    }

    //LOG(LOG_PRINT,4,"importing from port %d has ended",portId);

  }
}

/**
 * get access to the TCP state
 **/
tcpState_t TCP::getTCP_State(void) {
	return tcpState;
}

/**
 * export state to the neighbours
 **/
void TCP::exportState(script_t* script) {
    if (tcpState.gotScript) {
        for (uint8_t blockNo = 0; blockNo < *script->nblocks; blockNo++) {
            if (script->blockStateType[blockNo]==BT_ALGO) {
                uint8_t* blockState = (uint8_t*)(script->blocks[blockNo].state);
                LOG(LOG_TRANSP, 2,"Export state block %d (size %d)",blockNo, script->blocks[blockNo].stateSize);
                if (!sendBuffer(blockState, script->blocks[blockNo].stateSize, blockNo, MSG_NBR_STATE, ALLPORTS)) {
                    LOG(LOG_TRANSP, 2,"Failed exporting state");
                }
            }
        }
    }
}

/**
 * send a char(uint8_t) data buffer without retransmission
 * @param the buffer itself: an array of chars (uint8_t)
 * @param the buffer length
 * @param output ports
 * @param block number
 **/
uint8_t TCP::sendBuffer(uint8_t *buffer, uint32_t bufferLength, uint32_t blockNo, uint8_t type, uint8_t ports) {
	// format msg to send
    message_t msgToSend;
    msgToSend.type              = type;
    msgToSend.headerStartMarker = HEADER_START_MARKER;
    msgToSend.scriptVer         = mainState->scriptVer;

    if (type == MSG_NBR_STATE) {

        msgToSend.length = bufferLength + NBR_STATE_HEADER_SIZE + PACKET_HEADER_LENGTH;

        LOG(LOG_TRANSP, 3, "send blocknr: %ld (size %d)", blockNo, msgToSend.length);
        LOG(LOG_TRANSP, 3, "sizeof(NBR)=%d sizeof(PACKET)=%d", sizeof(nbrStateMessage_t), sizeof(message_t)-PAYLOADSIZE);

        msgToSend.payload[0] = blockNo & 0xff;
        msgToSend.payload[1] = (blockNo >> 8) & 0xff;
        
        LOG(LOG_TRANSP, 3, "payload 0: %d", msgToSend.payload[0]);
        LOG(LOG_TRANSP, 3, "payload 1: %d", msgToSend.payload[1]);

        // portrcv and portsnd are rewritten anyhow - ignore
        for (uint16_t i=0; i<bufferLength; ++i) {
            msgToSend.payload[4 + i] = buffer[i];
        }

    } else {
        msgToSend.length = bufferLength + PACKET_HEADER_LENGTH;

        for (uint8_t i=0; i<bufferLength; i++) {
            msgToSend.payload[i] = buffer[i];
        }
    }

    // insert checksum
    msgToSend.checksum = getMsgChecksum(&msgToSend);

    // send packet to all selected ports
    sendMsgToSerial(msgToSend, ports);
}

uint8_t TCP::sendPacket(uint8_t*, uint32_t, uint8_t, uint8_t) {

}

/**
 * The function sends a buffer to the serial port. It
 * retransmits packets in case of unsuccessful transmission.
 * @param the buffer itself: an array of chars (uint8_t)
 * @param the buffer length
 * @param last packet received by requesting node
 * @param output ports
 **/
uint8_t TCP::unicastFilePkt(uint8_t*,uint16_t,int16_t,uint8_t) {

}

/**
 * process a received message
 * @param input message
 * @param serial port number
**/
void TCP::processReceivedMessage(message_t*, uint8_t serialPortNo) {

}

/**
 * setup the algorithm
**/
void TCP::initTransport(void) {

}

/**
 * ask for a Script script
**/
void TCP::askForFile(void) {

}

/**
 * beacon a dummy message
**/
void TCP::beacon(void) {
	uint8_t payload[2];
    if (!sendBuffer(payload,2,0,MSG_BEACON,ALLPORTS)) {
        LOG(LOG_TRANSP,2,"Failed beaconing");
    }
}

/**
 * process the incoming script msg
 **/
void TCP::processIncomingScriptMsg(message_t *msg, uint8_t portId) {

}

void TCP::feedNbrStateMsgToAlgo(nbrStateMessage_t*) {

}

void TCP::startTimerInt() {

}

void TCP::stopTimerInt() {

}

/**
 * cleanup the TCP state
**/
void TCP::cleanupTCPstate(void) {

}

void TCP::initQueues(void) {
	for(uint8_t i = 0; i < SERIAL_PORT_COUNT; i++) {
		queues[i]->initQueue();
	}
}

// comparison between two script versions
uint8_t TCP::isScriptVerLarger(uint8_t newsv, uint8_t oldsv) {

}

// sends signal values around
void TCP::sendSignalValue(void) {

}

// de-allocate the new file
void TCP::deallocateNewFile(void) {

    // deallocate new script
    free(tcpState.newFile);
    tcpState.newFile = NULL;

    tcpState.prevFilePktIdx = -1;
    tcpState.askForFile = 1;
    tcpState.newFileSize = 0;
    tcpState.prevTCPevent = millis();
    
    initQueues();
}


