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

