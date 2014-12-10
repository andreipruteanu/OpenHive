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

#ifndef __tcp_h
#define __tcp_h

#include "types.h"
#include "queue.h"
#include "script.h"

class TCP {
private:

	// msg queue for the serial ports
	Queue* queues[SERIAL_PORT_COUNT];

	// script object
	Script* scriptHandler;

	// the state of the transport protocol
	tcpState_t tcpState;

	// pointer to the main state
	runtimeState_t* runtimeState;

protected:

public:
	/**
 	 * constructor
	 **/
	TCP(runtimeState_t* _runtimeState, Script* _scriptHandler);

	/**
 	 * destructor
	 **/
	~TCP(void);

	/**
 	 * get access to the TCP state
	 **/
	tcpState_t getTCP_State(void);

	/**
 	 * import state & script from the neighbours
	 **/
	void import(void);

	/**
     * export state to the neighbours
	 **/
	void exportState(void);

	/**
 	 * send a char(uint8_t) data buffer without retransmission
 	 * @param the buffer itself: an array of chars (uint8_t)
 	 * @param the buffer length
	 * @param output ports
	 * @param block number
	 **/
	uint8_t sendBuffer(uint8_t*,uint32_t,uint32_t,uint8_t,uint8_t);

	uint8_t sendPacket(uint8_t*, uint32_t, uint8_t, uint8_t);

	/**
 	 * The function sends a buffer to the serial port. It
 	 * retransmits packets in case of unsuccessful transmission.
 	 * @param the buffer itself: an array of chars (uint8_t)
 	 * @param the buffer length
 	 * @param last packet received by requesting node
 	 * @param output ports
	 **/
	uint8_t unicastScriptPkt(uint16_t,uint8_t);

	/**
	 * process a received message
	 * @param input message
	 * @param serial port number
	**/
	void processReceivedMessage(message_t*, uint8_t serialPortNo);

	/**
	 * setup the algorithm
	**/
	void initTCP(void);

	/**
	 * ask for a Script script
	**/
	void askForScript(void);

	/**
	 * beacon a dummy message
	**/
	void beacon(void);

	/**
	 * process the incoming script msg
	**/
	void processIncomingScriptMsg(message_t *msg, uint8_t portId);

	void feedNbrStateMsgToAlgo(nbrStateMessage_t*);

	void startTimerInt();
	void stopTimerInt();

	/**
	 * cleanup the TCP state
	**/
	void cleanupTCPstate(void);

	void initQueues(void);

	// comparison between two script versions
	uint8_t isScriptVerLarger(uint8_t newsv, uint8_t oldsv);

	// sends signal values around
	void sendSignalValue(void);

	void sendScriptVersion(void);

	void resetTCPState(void);

	void sendFirmwareVersion(void);
};

#endif

