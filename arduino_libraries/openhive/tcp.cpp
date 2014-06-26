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

extern uint8_t debuglevel[LOG_LASTENTRY];

TCP::TCP(mainState_t* _mainState, Script* _scriptHandler) {
	mainState = _mainState;
	scriptHandler = _scriptHandler;

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
                    LOG(LOG_TRANSP, 2, "Nbr has new ver: %d. Mine is: %d",newMsg.scriptVer,mainState->scriptVer);
                    resetTCPState();
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
void TCP::exportState(void) {
    if (tcpState.gotScript) {
        for (uint8_t blockNo = 0; blockNo < (scriptHandler->getBlockCount()); blockNo++) {
            if ((scriptHandler->getBlockStateTypes())[blockNo] == BT_ALGO) {
                uint8_t* blockState = (uint8_t*)((scriptHandler->getBlocks())[blockNo].state);
                LOG(LOG_TRANSP, 2,"Export state block %d (size %d)",blockNo, (scriptHandler->getBlocks())[blockNo].stateSize);
                if (!sendBuffer(blockState, (scriptHandler->getBlocks())[blockNo].stateSize, blockNo, MSG_NBR_STATE, ALLPORTS)) {
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

uint8_t TCP::sendPacket(uint8_t* buffer, uint32_t bufferLength, uint8_t type, uint8_t ports) {
    // format data to send
    message_t msgToSend;
    msgToSend.type              = type;    
    msgToSend.headerStartMarker = HEADER_START_MARKER; 
    msgToSend.length            = bufferLength + PACKET_HEADER_LENGTH;
    msgToSend.scriptVer         = mainState->scriptVer;

    for (uint8_t i=0; i<bufferLength; i++) {
        msgToSend.payload[i] = buffer[i];
    }

    // insert checksum
    msgToSend.checksum = getMsgChecksum(&msgToSend);

    // send packet to all selected ports
    sendMsgToSerial(msgToSend, ports);
}

/**
 * The function sends a buffer to the serial port. It
 * retransmits packets in case of unsuccessful transmission.
 * @param the buffer itself: an array of chars (uint8_t)
 * @param the buffer length
 * @param last packet received by requesting node
 * @param output ports
 **/
uint8_t TCP::unicastScriptPkt(uint16_t pkindex, uint8_t portId) {
    if (portId == ALLPORTS) {
        // unicast script to all ports???
        LOG(LOG_TRANSP, 2, "unicast script to all ports?");
        return -1;
    }

    uint16_t sentbytes = 0;
    uint16_t lengthtosend;
    int16_t packetindex = 0;
    uint16_t nrpackets;
    uint16_t payloadSize;
    uint16_t headerSize;
    uint16_t bufferLength = scriptHandler->getScriptSize();

    nrpackets   = ceil((float)bufferLength / SCRIPTPAYLOADSIZE);
    headerSize  = SCRIPT_HEADER_LENGTH + PACKET_HEADER_LENGTH;
    payloadSize = SCRIPTPAYLOADSIZE;

    LOG(LOG_TRANSP, 1, "-- script size: %d", bufferLength);
    LOG(LOG_TRANSP, 1, "-- number of packets: %d", nrpackets);

    while (sentbytes < bufferLength) {

        // do we have a full packet to send?
        if ((bufferLength - sentbytes) >= SCRIPTPAYLOADSIZE) {      
            lengthtosend = payloadSize;
        } else {
            lengthtosend = bufferLength - sentbytes;
        }

        if (packetindex > pkindex) {

            // format data to send
            message_t msgToSend;
            msgToSend.headerStartMarker = HEADER_START_MARKER; 
            msgToSend.length            = lengthtosend + headerSize;
            msgToSend.type              = MSG_SCRIPT;
            msgToSend.port = portId;

            scriptmessage_t *tempscriptmsg = (scriptmessage_t *)msgToSend.payload;
            tempscriptmsg->packetindex          = packetindex;
            tempscriptmsg->nrpackets            = nrpackets;
            tempscriptmsg->usedPayloadSize      = lengthtosend;
	        
            //copy a piece of the buffer into the pkt payload
            for (uint16_t i=0; i<lengthtosend; i++) {
                tempscriptmsg->payload[i] = (scriptHandler->getScriptArray())[sentbytes + i];
            }

            // mark the msg with the script version
            msgToSend.scriptVer = mainState->scriptVer;

            // compute and add the checksum
            msgToSend.checksum = getMsgChecksum(&msgToSend);

            sendMsgToSerial(msgToSend, portId);
            return 1;
        }

        // update the remaining segments to send
        sentbytes   = sentbytes + lengthtosend;
        packetindex = packetindex + 1;
    }

    return 1;
}

/**
 * process a received message
 * @param input message
 * @param serial port number
**/
void TCP::processReceivedMessage(message_t *msg, uint8_t portId) {

    LOG(LOG_TRANSP,2,"processing rcv type:%d port:%d",msg->type, portId);

    switch (msg->type) {

        case MSG_SCRIPT:
            LOG(LOG_TRANSP,3,"got script msg, asking: %d",tcpState.askForScript);
            // accept only msgs belonging to the new script
            if (tcpState.askForScript && (msg->scriptVer == mainState->newScriptVer)) {
                LOG(LOG_TRANSP,3,"proc. msg, ver:%d, mine: %d",msg->scriptVer,mainState->newScriptVer);
                processIncomingScriptMsg(msg, portId);
            } else {
                LOG(LOG_TRANSP,3,"!proc msg, ver:%d, mine: %d",msg->scriptVer,mainState->newScriptVer);               
            }
            break;

        case MSG_NBR_STATE: 
            {
                if (tcpState.gotScript && (msg->scriptVer == mainState->scriptVer)) {
                    LOG(LOG_TRANSP,3,"proc nbr state. nbr ver: %d, my ver: %d",msg->scriptVer, mainState->scriptVer);
                    nbrStateMessage_t nbrStateMsg;
                    nbrStateMsg.blockId = msg->payload[0] + (msg->payload[1] << 8); 
                    nbrStateMsg.portrcv = portId;
                    nbrStateMsg.portsnd = msg->port;
                    nbrStateMsg.payload = &msg->payload[4];

                    feedNbrStateMsgToAlgo(&nbrStateMsg);
                }
            } 
            break;

        case MSG_NEED_SCRIPT:
            LOG(LOG_TRANSP, 1, "--- received script request");
            if (tcpState.gotScript) {
                if (msg->payload[2] == mainState->scriptVer) {
                    // extract packet index
                    int16_t pkindex = 0;
                    pkindex = (msg->payload[0] << 8) + msg->payload[1] - 1;
                    LOG(LOG_TRANSP, 1, "-- began exporting script %d (pkt %d)",mainState->scriptVer,pkindex);
                    unicastScriptPkt(pkindex, portId);
                    LOG(LOG_TRANSP, 1, "-- done exporting script");
                } else {
                    LOG(LOG_TRANSP, 1, "script req issue: mine:%d, req:%d",mainState->scriptVer,msg->payload[2]);
                }
            }
            break;

        case MSG_DEALLOC:
            LOG(LOG_TRANSP,1,"received dealloc msg");
            LOG(LOG_PRINT,1,"!!! deallocating !!!!");
            mainState->lifecycleAction = LA_DEALLOCATE; // trigger "deallocate" action
            break;

        case MSG_REBOOT:
            LOG(LOG_TRANSP,1,"received reboot msg");
            LOG(LOG_PRINT,1,"!!! rebooting !!!!");
            mainState->lifecycleAction = LA_RESET_NODE; // trigger "reset" action
            break;
            
        case MSG_STATUS:
            LOG(LOG_TRANSP,1,"received show status req msg");
            mainState->lifecycleAction = LA_SHOW_STATUS; // trigger "show status ()" action
            break;

        case MSG_INFO:
            LOG(LOG_TRANSP, 2,"received log msg. ignoring.");
            break;
            
        case MSG_BEACON:
            LOG(LOG_TRANSP, 2,"received beacon msg. ignoring.");
            break;

        case MSG_SEND_SCRIPT_VER:
            LOG(LOG_TRANSP,2,"sending script ver");
            sendScriptVersion();
            break;

        case MSG_SCRIPT_VER:
            uint8_t scriptVer;
            scriptVer = msg->payload[0];
            LOG(LOG_TRANSP,2,"received script ver: %d",scriptVer);
            break;

        case MSG_SEND_FIRM_VER:
            LOG(LOG_TRANSP,2,"sending firmware version");
            sendFirmwareVersion();
            break;

        case MSG_FIRM_VER:
            {
                uint8_t i;
                uint32_t firmVer;

                // extract payload: {id (1 byte) - value (1 byte)}, ...
                uint8_t payloadSize = msg->length - PACKET_HEADER_LENGTH;

                uint8_t payload[payloadSize];

                for (i=0; i<payloadSize; ++i) {
                    payload[i] = msg->payload[i];
                }

                firmVer = *(uint32_t *)&payload;
                LOG(LOG_TRANSP,2,"nbr has firmware version: %ld",firmVer);
            }
            break;

        case MSG_LOGLEVEL:
            LOG(LOG_TRANSP, 2,"received loglevel msg");
            {
                uint32_t i;

                // extract payload: {id (1 byte) - value (1 byte)}, ...
                uint32_t payloadsize = msg->length - PACKET_HEADER_LENGTH;

                for (i=0; i<payloadsize; ++i) {
                    // read id
                    uint8_t id = msg->payload[i];

                    // read value
                    ++i;
                    uint8_t val = msg->payload[i];

                    // dummy test
                    if (id >= LOG_LASTENTRY) {
                        LOG(LOG_TRANSP, 1,"loglevel value exceeds maximum limit");
                    } else {
                        // modify logging variable
                        debuglevel[id] = val;
                    }
                }
            }
            break;

        case MSG_GET_SIGNAL:
            mainState->reportSignalId[0] = (int32_t)((msg->payload[0]<<24)  + (msg->payload[1]<<16)  + (msg->payload[2]<<8)  + msg->payload[3]);
            mainState->reportSignalId[1] = (int32_t)((msg->payload[4]<<24)  + (msg->payload[5]<<16)  + (msg->payload[6]<<8)  + msg->payload[7]);
            mainState->reportSignalId[2] = (int32_t)((msg->payload[8]<<24)  + (msg->payload[9]<<16)  + (msg->payload[10]<<8) + msg->payload[11]);
            mainState->reportSignalId[3] = (int32_t)((msg->payload[12]<<24) + (msg->payload[13]<<16) + (msg->payload[14]<<8) + msg->payload[15]);
            mainState->reportSignalIdPort = portId;
            LOG(LOG_TRANSP, 2,"msg signal: %ld %ld %ld %ld", mainState->reportSignalId[0], mainState->reportSignalId[1], mainState->reportSignalId[2], mainState->reportSignalId[3]);
            break;
            
        case MSG_SEND_SIGNAL:
            LOG(LOG_TRANSP, 2,"received signal value message. ignoring.");
            break;

        default:
            LOG(LOG_TRANSP, 2,"un-recognised pkt type: %d.",msg->type);
            break;
    }
}

/**
 * setup the algorithm
 **/
void TCP::initTCP(void) {
    initQueues();
    
    // initialize script related vars
    tcpState.prevScriptPktIdx = -1;
    tcpState.gotScript = 0;
    tcpState.gotInputMsgs = 0;
    tcpState.askForScript = 1;
    tcpState.prevTCPevent = millis();

    // init timers
    tcpState.prevTCPevent = millis();
}

/**
 * ask for a Script script
 **/
void TCP::askForScript(void) {

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

// de-allocate the new script
void TCP::resetTCPState(void) {

    tcpState.prevScriptPktIdx = -1;
    tcpState.askForScript = 1;
    tcpState.prevTCPevent = millis();
    
    initQueues();
}

// send the current script version
void TCP::sendScriptVersion(void)
{
    uint8_t payload[1];
    payload[0] = mainState->scriptVer;
    if (!sendPacket(payload,1,MSG_SCRIPT_VER,ALLPORTS)) {
        LOG(LOG_TRANSP,2,"Failed sending script version");
    }
}

// send the current firmware version
void TCP::sendFirmwareVersion(void)
{
    uint8_t payload[4];
    
    payload[0] =  mainState->firmVer        & 0xff;
    payload[1] = (mainState->firmVer >> 8)  & 0xff;
    payload[2] = (mainState->firmVer >> 16) & 0xff;
    payload[3] = (mainState->firmVer >> 24) & 0xff;
    
    //*((uint8_t*)payload) = mainState.firmVer;
    LOG(LOG_TRANSP, 2, "Sending firmVer: %ld", mainState->firmVer);

    if (!sendPacket(payload, 4, MSG_FIRM_VER, ALLPORTS)) {
        LOG(LOG_TRANSP,2,"Failed sending firmware version");
    }
}




