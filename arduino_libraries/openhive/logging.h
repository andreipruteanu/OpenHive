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

#ifndef __LOGGING_H
#define __LOGGING_H

#include "Arduino.h"
#include "TCP.h"
#include "types.h"
#include "block_factory.h"
#include <stdio.h>
#include <stdarg.h>

// the log markers for every module
typedef enum LOG_BLOCK_type {
  LOG_ACTUATOR,
  LOG_ABS,
  LOG_EQ,
  LOG_SIM,
  LOG_ADD,
  LOG_SUB,
  LOG_NEG,
  LOG_INV,
  LOG_FAC,
  LOG_MOD,
  LOG_INTDIV,
  LOG_ANDGATE,
  LOG_NANDGATE,
  LOG_ORGATE,
  LOG_NORGATE,
  LOG_XORGATE,
  LOG_XNORGATE,
  LOG_SIN,
  LOG_MAJGATE,
  LOG_POW,
  LOG_DIV,
  LOG_LARGERTHAN,
  LOG_SMALLERTHAN,
  LOG_MUL,
  LOG_NOTGATE,
  LOG_SENSOR,
  LOG_TOKEN,
  LOG_MAIN,
  LOG_SCRIPT,
  LOG_PARSER,
  LOG_TRANSP,
  LOG_ALGRAND,
  LOG_ALGGRAD,
  LOG_ALGSYNC,
  LOG_BOGUS,
  LOG_TEST,
  LOG_DEAL,
  LOG_DELAY,
  LOG_HOLD,
  LOG_Q,
  LOG_TICK,
  LOG_WDT,
  LOG_HSL,
  LOG_PRINT,          // always active
  LOG_COMBINE,
  LOG_SCALE,
  LOG_LOGIC,
  LOG_FLASHWRITE,
  LOG_FLASHREAD,
  LOG_LASTENTRY
} LOG_BLOCK;

// init logging constants
void InitLogging(void);

// the actual log function
void LOG(LOG_BLOCK lb, int loglevel, const char *format, ...);

// compute checksum of the serial msg
uint8_t getMsgChecksum(message_t *m);

// send msg to the serial port
void sendMsgToSerial(message_t, uint8_t);

#endif
