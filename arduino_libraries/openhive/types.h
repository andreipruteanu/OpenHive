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

#ifndef __TYPES_H
#define __TYPES_H

//#include "servo.h"

#include "Arduino.h"

/**
 * \def enable debugging
 */
#define DEBUG

/**
 * \def the firmware version
**/
#ifndef FIRMWARE_VERSION
    #define FIRMWARE_VERSION 0
#endif

 /**
 * \def maximum execution time
**/
#ifndef MAX_EXECUTION_TIME
    #define MAX_EXECUTION_TIME 60
#endif

/**
 * \def release level
**/
#ifndef RELEASE_LEVEL
    #define RELEASE_LEVEL 2
#endif

 /**
 * \def the serial bus speed
**/
#define SERIAL_BUS_SPEED 115200

/**
 * \def the size of the packet header
**/
#define PACKET_HEADER_LENGTH 6

/**
 * \def number of bytes occupied by header in the script packet
**/
#define SCRIPT_HEADER_LENGTH 3

/**
 * \struct data structure holding info received from a neighboring node
**/
typedef struct {
    uint16_t blockId;                 /**< the block for which this packet is intended for */
    uint8_t portrcv;
    uint8_t portsnd;
    uint8_t *payload;                 /**< payload of the state packet */
} nbrStateMessage_t;

/**
 * \def number of bytes occupied by header in the nbr packet
**/
#define NBR_STATE_HEADER_SIZE sizeof(nbrStateMessage_t)

/**
 * \def payload size of a packet in bytes
**/
#define PAYLOADSIZE 55

/**
 * \def msg(packet) length
**/
#define MSG_LENGTH (PACKET_HEADER_LENGTH + PAYLOADSIZE)

/**
 * \def SCRIPT payload size
**/
#define SCRIPTPAYLOADSIZE (PAYLOADSIZE - SCRIPT_HEADER_LENGTH)

/**
 * \def NBR state payload size
**/
#define NBRPAYLOADSIZE (PAYLOADSIZE - NBR_STATE_HEADER_SIZE)

/**
 * \def maximum number of attempts to send a packet before stopping
**/
#define MAXRETRANSMIT 5

/**
 * \def maximum script file size
**/
#define MAX_SCRIPT_SIZE 2000

/**
 * \def maximum nbr state array size
**/
#define MAX_NBR_STATE_SIZE 100

/**
 * \def serial listen time
**/
#define SERIAL_LISTEN_TIME 10

/**
 * \def number of serial ports
**/
#define SERIAL_PORT_COUNT 4

/**
 * \def bitmask defining the used ports
**/
#define ALLPORTS 0xfe

/**
 * \def bitmask for the debug port
**/
#define PORTDEBUG SERIAL_PORT_COUNT

/**
 * the time intervals between listening for neighbour state updates
**/
#define IMPORT_PERIOD 100

/**
 * the time interval between export state actions
 **/
#define EXPORT_STATE_PERIOD 500

/**
 * the time interval between export SCRIPT actions
 **/
#define ASK_FOR_SCRIPT_PERIOD 500

/**
 * the time intervals between cleaning the TCP state
 **/
#define TCP_CLEANUP_PERIOD IMPORT_PERIOD

/**
 * the time interval after which the TCP state expires
 **/
#define TCP_EXPIRE_TIME EXPORT_STATE_PERIOD * 6

/**
 * the default script size: in case there should be a default script on the nodes
 **/
#define DEFAULT_SCRIPT_SIZE 0

/**
 * define the header start marker
 **/
#define HEADER_START_MARKER 254

/**
 * serial sampling frequency in Hz
 **/
#define SERIAL_SAMPLING_FREQ 20

/**
 * the incoming message queue length
 **/
#define MSG_QUEUE_PKT_SIZE 10

/**
 * the size of the message queue in bytes
 **/
#define MSG_QUEUE_SIZE ((MSG_QUEUE_PKT_SIZE * MSG_LENGTH) + 1)

/**
 * the dummy value used for having 4 bytes in the header
 **/
#define DUMMY_VALUE 5

/**
 * \def size of execution round
 **/
#define EXECUTE_PERIOD 200


/**
 * \def size of beconing interval in milliseconds
**/
#define BEACON_PERIOD   (EXPORT_STATE_PERIOD) * 2

/**
 * \def number of ticks after which a msgs is removed from the queue
**/
#define MSG_EXPIRE_TIME 10

/**
 * \def number of ticks after which the gradient pkts are expired
**/
#define GRADIENT_MAX_PKT_LIFETIME 2

/**
 * \def number of ticks after which the gradient expires
**/
#define GRADIENT_EXPIRE_TIME 30


/**
 * \def defines the token that signals the start signal value
**/
#define TOK_SIG_VAL 1

/**
 * \def defines the token that signals the absence of a signal value
**/
#define TOK_SIG_NOVAL 0

/**
 * \def defines the size of the signal value in bytes
**/
#define SIZE_SIG_VAL 4

/**
 * \def Watchdog period 5000ms
**/
#define WDT_PERIOD 5000


/**
 * \struct generic packet structure
**/
typedef struct {
    uint8_t headerStartMarker;    /**< packet start marker */
    uint8_t length;               /**< length of the whole message */
    uint8_t type;                 /**< packet type */
    uint8_t port;                 /**< sender port id */
    uint8_t checksum;             /**< checksum of the packet - simple xor among payload bytes */
    uint8_t scriptVer;            /**< the script version */
    uint8_t payload[PAYLOADSIZE]; /**< actual payload */
} message_t;



/**
 * \struct data structure holding info received in a script message
**/
typedef struct {
    uint8_t nrpackets;                /**< the number of packets in a SCRIPT stream */
    uint8_t packetindex;              /**< the packet index in a SCRIPT stream */
    uint8_t usedPayloadSize;          /**< the number of bytes used in a packet */
    uint8_t payload[SCRIPTPAYLOADSIZE]; /**< the SCRIPT packet payload */
} scriptmessage_t;

enum {
    TEST_FAILED = 0,
    TEST_PASSED
};

typedef struct {
    uint16_t tick;              // tick where the test failed
    uint16_t sigid;             // the id of the signal that failed the test
    float value;                // the recorded value of the signal
    uint8_t status;             // test failed, all tests passed
} testmessage_t;


/**
 * \enum message types for our network
**/
enum {
   MSG_SCRIPT = 0,      /**< message script type                  */
   MSG_NBR_STATE,       /**< message neighbour state Type         */
   MSG_NEED_SCRIPT,     /**< message Need Script type             */
   MSG_INFO,            /**< message with text information        */
   MSG_TEST,            /**< message used in unittesting          */
   MSG_DEALLOC,         /**< message used in cleaning the memory  */
   MSG_REBOOT,          /**< message used in rebooting the system */
   MSG_LOGLEVEL,        /**< message used in the logging system   */
   MSG_BEACON,          /**< message used for beaconing           */
   MSG_STATUS,          /**< message node status                  */
   MSG_SEND_SCRIPT_VER, /**< message send script version          */
   MSG_SEND_FIRM_VER,   /**< message send firmware version        */
   MSG_SCRIPT_VER,      /**< message script version               */
   MSG_FIRM_VER,        /**< message firmware version             */
   MSG_GET_SIGNAL,      /**< message requesting the value of a signal    */
   MSG_SEND_SIGNAL,     /**< message replying with the value of a signal */
   MSG_LAST
};

/**
 * release level
**/
enum {
    RELEASE_FREEWARE = 0, /**<  */
    RELEASE_PARTIAL,      /**<  */
    RELEASE_FULL          /**<  */
};

typedef struct {
    void (*init) (uint16_t);
    void (*in)   (uint16_t);
    void (*out)  (uint16_t);
    void (*step) (uint16_t);
    void (*deal) (uint16_t);
    uint16_t stateSize;         // size of state
    void* state;                // pointer to state struct
} block_t;

/**
 * \enum defines the types of wires
**/
enum wireType {
    WT_SIGNAL,
    WT_CONSTANT
};

/**
 * \enum define types of supported blocks
**/
enum {
    SYNC     = 0,            // DONE
    GRADIENT,                // DONE
    ALG_TOKEN,               // DONE
    ALG_RANDOM,              // DONE
    ADDITION = 20,           // DONE
    MULTIPLICATION = 21,     // DONE
    DIVISION = 22,           // DONE
    SUBSTRACTION,            // DONE
    NEGATIVE,                // DONE
    POWER,                   // DONE
    ABSOLUTE,                // DONE
    INTDIV,                  // DONE
    MODULUS,                 // DONE
    FACTORIAL,               // DONE
    EQUALITY,                // DONE
    SIMILARITY,              // DONE
    SMALLERTHAN,             // DONE
    LARGERTHAN = 33,         // DONE
    GATEOR,                  // DONE
    GATEAND = 35,            // DONE
    GATENOT = 36,            // DONE
    GATEXOR,                 // DONE
    GATENAND,                // DONE
    GATENOR,                 // DONE
    GATEMAJORITY,            // DONE
    GATEXNOR,                // DONE
    SIN,                     // DONE
    SENSOR   = 50,           // DONE
    ACTUATOR = 52,           // DONE
    HOLD = 53,               // DONE
    TICK = 54,               // DONE
    DELAY = 55,              // DONE
    HSL2RGB = 58,            // DONE
    COMBINE = 59,            // DONE
    SCALE = 60,              // DONE
    LOGIC = 61,              // DONE
    FLASHWRITE = 62,         // DONE
    FLASHREAD = 63,          // DONE
    LASTBLOCK
};

// block types
enum {
    BT_SIMPLE,
    BT_STATE,
    BT_ALGO
};

/**
 * \struct defines the global state
**/
typedef struct {

    // release level: 0 freeware, 1 partially featured, 2 fully featured
    uint8_t releaseLevel;

    // define the max debug level: 0 for "release" mode
    uint8_t DEBUG_LEVEL;

    // elapsed time
    uint8_t eventImport;
    uint32_t lastEventImport;
    uint32_t lastEventExecute;
    uint32_t lastEventAskScript;
    uint32_t lastEventBeacon;
    uint32_t lastEventWDTRestart;
    uint32_t lastServoEvent;

    // the current tick
    uint32_t tick;

    // "is memory allocated" flag
    uint8_t memAlloc;

    // script version
    uint8_t scriptVer;

    // new script version
    uint8_t newScriptVer;

    // firmware version
    uint32_t firmVer;

    // watchdog timer mode
    uint32_t wdtMode;

    // watchdog timer expire period
    uint32_t wdtTimeout;

    // maxExecutionTime
    uint32_t maxExecutionTime;

    // reporting signal id
    int32_t reportSignalId[4];
    int32_t reportSignalIdPort;

    // lifecycle action
    uint8_t lifecycleAction;

} mainState_t;

enum {
    LA_NONE,
    LA_DEALLOCATE,
    LA_RESET_NODE,
    LA_SHOW_STATUS
};

typedef struct {
    uint8_t p[256];
} pinState_t;

typedef struct {
    uint16_t* in;
    uint16_t* out;
} ports_t;

typedef struct {
    uint16_t* nblocks;
    uint16_t* nsignals;
    block_t*  blocks;
    float* signals;
    uint16_t* blockTypes;
    ports_t* ports;
    uint8_t* blockStateType;
    uint16_t* nrinitsigs;
} script_t;

/**
 * \struct defines the transport protocol state
**/
typedef struct {
    uint32_t timeLastScriptPkt;
    uint32_t timeScriptReceived;
    uint32_t prevTCPevent;
    int16_t prevScriptPktIdx;
    uint8_t gotScript;    
    uint8_t gotInputMsgs;
    uint8_t askForScript;
    uint8_t hasDefaultScript;
} tcpState_t;

#define BUFFER_SIZE (5*MSG_LENGTH)

typedef struct {
    void (*initfunc) (uint16_t);
    void (*infunc)   (uint16_t);
    void (*outfunc)  (uint16_t);
    void (*stepfunc) (uint16_t);
    void (*dealfunc) (uint16_t);
    uint8_t* state;
    uint16_t type;                // type of block: ANDGATE, etc.
    uint16_t stateSize;
    uint16_t nin;
    uint16_t nout;
    uint8_t statetype;            // BT_SIMPLE, STATE, ALGO
} blockinfo_t;

typedef struct {
    uint16_t time;
    uint16_t sigid;
    float value;
} testvec_t;

typedef struct {
    uint16_t time;
    uint16_t blockType;
    void *state;
} testmessagevector_t;

typedef struct {
    uint32_t starttick;
    uint16_t posinvec;
    uint16_t posoutvec;
    uint16_t posmsgvec;
    uint16_t *ninvec;
    uint16_t *noutvec;
    testvec_t *invec;
    testvec_t *outvec;
    uint16_t *nmsgvec;
    testmessagevector_t **msgvec;
} testscript_t;

/*
#define SERVO_UPDATE_PERIOD 15

typedef struct  {
  uint8_t nbr        :6 ;             // a pin number from 0 to 63
  uint8_t isActive   :1 ;             // true if this channel is enabled, pin not pulsed if false
} ServoPin_t   ;

typedef struct {
  ServoPin_t Pin;
  volatile unsigned int ticks;
} servo_t;

typedef struct {
  uint8_t ServoCount;
  servo_t servos[MAX_SERVOS];
  volatile int8_t Channel[_Nbr_16timers];        // counter for the servo being pulsed for each timer (or -1 if refresh interval)
  int8_t min[MAX_SERVOS];                        // minimum is this value times 4 added to MIN_PULSE_WIDTH
  int8_t max[MAX_SERVOS];                        // maximum is this value times 4 added to MAX_PULSE_WIDTH
  uint8_t value[MAX_SERVOS];                     // value between 0 and 180
} servoState_t;
*/
#endif
