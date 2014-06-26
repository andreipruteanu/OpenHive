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
