#ifndef __tcp_h
#define __tcp_h

#include "types.h"

class TCP {
private:
protected:
public:
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
	uint8_t unicastFilePkt(uint8_t*,uint16_t,int16_t,uint8_t);

	/**
	 * process a received message
	 * @param input message
	 * @param serial port number
	**/
	void processReceivedMessage(message_t*, uint8_t serialPortNo);

	/**
	 * setup the algorithm
	**/
	void initTransport(void);

	/**
	 * ask for a Script script
	**/
	void askForFile(void);

	/**
	 * beacon a dummy message
	**/
	void beacon(void);

	/**
	 * compute the checksum for the message payload
	**/
	uint8_t getChecksum(message_t*);
	void deliverSerial(message_t, uint8_t);

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

	void startUSARTInt0();
	void stopUSARTInt0();
	void startUSARTInt1();
	void stopUSARTInt1();
	void startUSARTInt2();
	void stopUSARTInt2();
	void startUSARTInt3();
	void stopUSARTInt3();

	void copyUartData();
	void startUARTInt();
	void stopUARTInt();

	void initQueues(void);

	// comparison between two script versions
	uint8_t isScriptVerLarger(uint8_t newsv, uint8_t oldsv);

	// sends signal values around
	void sendSignalValue(void);
};

#endif