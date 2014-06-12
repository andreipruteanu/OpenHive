#include "logging.h"
#include "tcp.h"
#include "types.h"

//extern mainState_t mainState;

extern TCP* tcpHandle;

// variable for selecting the debugging level
uint8_t debuglevel[LOG_LASTENTRY];

void InitLogging() {



    // init all entries to 0
    for (int i=0;i<LOG_LASTENTRY;i++) {
      debuglevel[i] = 0;
    }

    // always active component - use it instead of serial.print
    debuglevel[LOG_PRINT] = 0xff;
}

void LOG(LOG_BLOCK lb, int loglevel, const char *format, ...)
{
/*
 * \def check wether the DEBUG is enabled
**/
#ifdef DEBUG

  char buffer[128];
  va_list args;
  va_start (args, format);
  int maxsize = vsnprintf(buffer,128,format, args);
  va_end (args);

  if (debuglevel[lb] >= loglevel) {

    // create the message
    message_t msg;

    if (maxsize>PAYLOADSIZE) {
      maxsize = PAYLOADSIZE;
    }

    msg.headerStartMarker = HEADER_START_MARKER;
    msg.length = maxsize + PACKET_HEADER_LENGTH;
    msg.type = MSG_INFO;
    msg.port = 0;
//    msg.scriptVer = mainState.scriptVer;
    for (int i=0; i<maxsize; i++) {
      msg.payload[i] = buffer[i];
    }

    msg.checksum = tcpHandle->getChecksum(&msg);

    // send only to port debug
    tcpHandle->deliverSerial(msg, PORTDEBUG);
  }

#endif

}

