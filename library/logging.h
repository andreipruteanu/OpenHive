#ifndef __LOGGING_H
#define __LOGGING_H

#include "Arduino.h"
#include "TCP.h"
#include <stdio.h>
#include <stdarg.h>

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

void InitLogging(void);
void LOG(LOG_BLOCK lb, int loglevel, const char *format, ...);


#endif
