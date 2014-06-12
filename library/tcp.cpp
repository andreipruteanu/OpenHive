#include "tcp.h"

void TCP::import(void) {
	
}

/**
 * export state to the neighbours
 **/
void TCP::exportState(void) {

}

/**
 * send a char(uint8_t) data buffer without retransmission
 * @param the buffer itself: an array of chars (uint8_t)
 * @param the buffer length
 * @param output ports
 * @param block number
 **/
uint8_t TCP::sendBuffer(uint8_t*,uint32_t,uint32_t,uint8_t,uint8_t) {

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

}

/**
 * compute the checksum for the message payload
**/
uint8_t TCP::getChecksum(message_t*) {

}

void TCP::deliverSerial(message_t, uint8_t) {

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


void TCP::copyUartData() {

}

void TCP::startUARTInt() {

}

void TCP::stopUARTInt() {

}

void TCP::initQueues(void) {

}

// comparison between two script versions
uint8_t TCP::isScriptVerLarger(uint8_t newsv, uint8_t oldsv) {

}

// sends signal values around
void TCP::sendSignalValue(void) {

}
