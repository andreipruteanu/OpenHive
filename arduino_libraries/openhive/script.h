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

#ifndef __script_h
#define __script_h

#include "types.h"

// information about the blocks
const blockinfo_t blockDetails[] = 
{ /*
	{initGradient , inGradient , outGradient , stepGradient , dealGradient  , NULL, GRADIENT       ,  sizeof(gradientState_t), 2, 1, BT_ALGO  },  // Alg. Gradient
	{initAlgToken , inAlgToken , outAlgToken , stepAlgToken , dealAlgToken  , NULL, ALG_TOKEN      ,  sizeof(AlgTokenState)  , 5, 1, BT_ALGO  },  // Alg. Token
	{initAlgRandom, inAlgRandom, outAlgRandom, stepAlgRandom, dealAlgRandom , NULL, ALG_RANDOM     ,  sizeof(AlgRandomState) , 1, 1, BT_ALGO  },  // Alg. Random
	{initSync     , inSync     , outSync     , stepSync     , dealSync      , NULL, SYNC           ,  sizeof(SyncState)      , 1, 2, BT_ALGO  },  // Alg. Sync	
	{initActuator , NULL       , outActuator , NULL         , dealActuator  , NULL, ACTUATOR       ,  sizeof(ActuatorState)  , 4, 1, BT_STATE },  // Actuator
	{initSensor   , NULL       , outSensor   , NULL         , dealSensor    , NULL, SENSOR         ,  sizeof(SensorState)    , 3, 1, BT_STATE },  // Sensor		
	{initHold     , NULL       , outHold     , NULL         , dealHold      , NULL, HOLD           ,  sizeof(HoldState)      , 2, 1, BT_STATE },  // Hold
	{initDelay    , NULL       , outDelay    , NULL         , dealDelay     , NULL, DELAY          ,  sizeof(DelayState)     , 2, 1, BT_STATE },  // Delay
	{initTick     , NULL       , outTick     , NULL         , dealTick      , NULL, TICK           ,  sizeof(TickState)      , 2, 1, BT_STATE },  // Tick
	{initLogic    , NULL       , outLogic    , NULL         , dealLogic     , NULL, LOGIC          ,  sizeof(LogicState)     , 5, 2, BT_STATE },  // Logic
	{initFlashWrite, NULL      , outFlashWrite, NULL        , dealFlashWrite, NULL, FLASHWRITE     ,  sizeof(FlashWriteState), 9, 0, BT_STATE },  // FlashWrite
	{NULL         , NULL       , flashread_  , NULL         , NULL          , NULL, FLASHREAD      ,  0                      , 0, 8, BT_SIMPLE},  // FlashRead
	{NULL         , NULL       , add_        , NULL         , NULL          , NULL, ADDITION       ,  0                      , 2, 1, BT_SIMPLE},  // ADDITION	
	{NULL         , NULL       , mul_        , NULL         , NULL          , NULL, MULTIPLICATION ,  0                      , 2, 1, BT_SIMPLE},  // MULTIPLICATION
	{NULL         , NULL       , div_        , NULL         , NULL          , NULL, DIVISION       ,  0                      , 2, 1, BT_SIMPLE},  // DIVISION	
	{NULL         , NULL       , sub_        , NULL         , NULL          , NULL, SUBSTRACTION   ,  0                      , 2, 1, BT_SIMPLE},  // SUBSTRACTION		
	{NULL         , NULL       , neg_        , NULL         , NULL          , NULL, NEGATIVE       ,  0                      , 1, 1, BT_SIMPLE},  // INVERSION
	{NULL         , NULL       , pow_        , NULL         , NULL          , NULL, POWER          ,  0                      , 2, 1, BT_SIMPLE},  // POWER	
	{NULL         , NULL       , abs_        , NULL         , NULL          , NULL, ABSOLUTE       ,  0                      , 1, 1, BT_SIMPLE},  // ABSOLUTE
	{NULL         , NULL       , intdiv_     , NULL         , NULL          , NULL, INTDIV         ,  0                      , 2, 1, BT_SIMPLE},  // INTDIV			
	{NULL         , NULL       , mod_        , NULL         , NULL          , NULL, MODULUS        ,  0                      , 2, 1, BT_SIMPLE},  // MODULUS
	{NULL         , NULL       , fac_        , NULL         , NULL          , NULL, FACTORIAL      ,  0                      , 1, 1, BT_SIMPLE},  // FACTORIAL	
	{NULL         , NULL       , eq_         , NULL         , NULL          , NULL, EQUALITY       ,  0                      , 2, 2, BT_SIMPLE},  // EQUALITY
	{NULL         , NULL       , sim_        , NULL         , NULL          , NULL, SIMILARITY     ,  0                      , 3, 2, BT_SIMPLE},  // SIMILARITY	
	{NULL         , NULL       , st_         , NULL         , NULL          , NULL, SMALLERTHAN    ,  0                      , 2, 2, BT_SIMPLE},  // SMALLER THAN
	{NULL         , NULL       , lt_         , NULL         , NULL          , NULL, LARGERTHAN     ,  0                      , 2, 2, BT_SIMPLE},  // LARGER THAN	
	{NULL         , NULL       , and_        , NULL         , NULL          , NULL, GATEAND        ,  0                      , 2, 1, BT_SIMPLE},  // AND GATE
	{NULL         , NULL       , or_         , NULL         , NULL          , NULL, GATEOR         ,  0                      , 2, 1, BT_SIMPLE},  // OR  GATE
	{NULL         , NULL       , not_        , NULL         , NULL          , NULL, GATENOT        ,  0                      , 1, 1, BT_SIMPLE},  // NOT GATE
	{NULL         , NULL       , xor_        , NULL         , NULL          , NULL, GATEXOR        ,  0                      , 2, 1, BT_SIMPLE},  // XOR GATE
	{NULL         , NULL       , nand_       , NULL         , NULL          , NULL, GATENAND       ,  0                      , 2, 1, BT_SIMPLE},  // NAND GATE
	{NULL         , NULL       , nor_        , NULL         , NULL          , NULL, GATENOR        ,  0                      , 2, 1, BT_SIMPLE},  // NOR GATE	
	{NULL         , NULL       , maj_        , NULL         , NULL          , NULL, GATEMAJORITY   ,  0                      , 3, 1, BT_SIMPLE},  // GATE MAJORITY
	{NULL         , NULL       , xnor_       , NULL         , NULL          , NULL, GATEXNOR       ,  0                      , 2, 1, BT_SIMPLE},  // GATE XNOR
	{NULL         , NULL       , sin_        , NULL         , NULL          , NULL, SIN            ,  0                      , 1, 1, BT_SIMPLE},  // TRIGONOMETRIC SINUS
	{NULL         , NULL       , hsl2rgb     , NULL         , NULL          , NULL, HSL2RGB        ,  0                      , 3, 3, BT_SIMPLE},  // HSL to RGB
	{NULL         , NULL       , combine_    , NULL         , NULL          , NULL, COMBINE        ,  0                      , 3, 1, BT_SIMPLE},  // COMBINE
	{NULL         , NULL       , scale_      , NULL         , NULL          , NULL, SCALE          ,  0                      , 5, 1, BT_SIMPLE},  // Scale condition
	{NULL         , NULL       , NULL        , NULL         , NULL          , NULL, LASTBLOCK      ,  0                      , 0, 0, BT_SIMPLE}   // terminator mark 
	*/
};

class Script {
private:

	uint16_t scriptSize;

    uint16_t blockCount;
    uint16_t signalCount;
    block_t*  blocks;
    float* signals;
    uint16_t* blockTypes;
    ports_t* ports;
    uint8_t* blockStateTypes;
    uint16_t initSigsCount;

	// script arrays
    uint8_t* scriptArray;
    uint8_t* newScriptArray;

    // script size
    uint16_t scriptArraySize;
    uint16_t newScriptArraySize;

	// pointer to the main state
	mainState_t* mainState;

	// 
	uint8_t defaultScriptArray[DEFAULT_SCRIPT_SIZE];// = {}; // change for default script

protected:

public:
	/**
 	 * constructor
	 **/
	Script(mainState_t* _mainState);

	/**
 	 * destructor
	 **/
	~Script(void);

	/**
 	 * get a handler to the script array
	 **/
	uint8_t* getScriptArray(void);

	/**
 	 * get script size in bytes
	 **/
	uint16_t getScriptSize(void);

	/**
 	 * report the number of blocks
	 **/
	uint16_t getBlockCount(void);

	/**
 	 * report the number of signals
	 **/
	uint16_t getSignalCount(void);

	/**
 	 * report the number of signals
	 **/
	uint16_t getInitSigsCount(void);

	/**
 	 * report block types
	 **/
	uint16_t* getBlockTypes(void);

	/**
 	 * report block state types
	 **/
	uint8_t* getBlockStateTypes(void);	

	/**
 	 * report blocks
	 **/
	block_t* getBlocks(void);

	/**
 	 * report signals
	 **/	
	float* getSignals(void);

	/**
 	 * report ports
	 **/	
	ports_t* getPorts(void);

	/**
	 * get block info
	 **/
	const blockinfo_t* getBlockInfo(uint16_t blocktype);

	/**
 	 * de-allocate signals
	 **/
	void deallocateSignals(void);

	/**
 	 * de-allocate ports
	 **/
	void deallocatePorts(void);

	/**
 	 * de-allocate blocks
	 **/
	void deallocateBlocks(void);

	/**
	 * de-allocate block types
	 **/
	void deallocateBlockTypes(void);

	/**
	 * de-allocate script array
	 **/
	void deallocateScriptArray(void);

	/**
	 * de-allocate new script array
	 **/
	void deallocateNewScriptArray(void);
};

#endif
