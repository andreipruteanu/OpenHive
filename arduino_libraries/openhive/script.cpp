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

#include "tcp.h"
#include "logging.h"
#include "types.h"
#include "script.h"

Script::Script(runtimeState_t* _runtimeState)
{

    runtimeState = _runtimeState;

    // define the default script
    uint8_t defaultScriptArray[DEFAULT_SCRIPT_SIZE] = {}; // change for default script

    // if default script is present,
    if (DEFAULT_SCRIPT_SIZE > 0) {
        
        scriptArray = (uint8_t*)malloc(DEFAULT_SCRIPT_SIZE * sizeof(uint8_t));

        for (uint16_t i = 0; i < DEFAULT_SCRIPT_SIZE; i++) {
            scriptArray[i] = defaultScriptArray[i];
        }

    }
}

/**
 * get a handler to the script array
 **/
uint8_t* Script::getScriptArray(void) {
    return scriptArray;
}

/**
 * get script size in bytes
 **/
uint16_t Script::getScriptSize(void) {
    return scriptSize;
}

/**
 * report the number of blocks
 **/
uint16_t Script::getBlockCount(void) {
    return blockCount;
}

/**
 * report the number of signals
 **/
uint16_t Script::getSignalCount(void) {
    return signalCount;
}

uint16_t Script::getInitSigsCount(void) {
    return initSigsCount;
}

/**
 * report the block types
 **/
uint16_t* Script::getBlockTypes(void) {
    return blockTypes;
}

/**
 * report block state types
 **/
uint8_t* Script::getBlockStateTypes(void) {
    return blockStateTypes;
}

/**
 * report blocks
 **/
Block** Script::getBlocks(void) {
    return blocks;
}

/**
 * report blocks
 **/
float* Script::getSignals(void) {
    return signals;
}

/**
 * report ports
 **/    
ports_t* Script::getPorts(void) {
    return ports;
}

// return static info about blocks
const blockinfo_t* Script::getBlockInfo(uint16_t blocktype) {
  
  uint16_t i = 0;
  
  LOG(LOG_SCRIPT, 3, "blockid: %d", blocktype);
  
  while (blockDetails[i].type != LASTBLOCK) {
  
    //LOG(LOG_SCRIPT, 4, "typeinstruct:%d", blockDetails[i].type);
    
    if (blockDetails[i].type == blocktype) {
      return &blockDetails[i];
    }
    
    i++;
  }
  
  return NULL;
}

void Script::deallocateSignals(void) {
    free(signals);
    signals = NULL;
}

void Script::deallocatePorts(void) {
    free(ports);
    ports = NULL;
}

void Script::deallocateBlocks(void) {
    free(blocks);
    blocks = NULL;   
}

void Script::deallocateBlockTypes(void) {
    free(blockTypes);
    blockTypes = NULL;   
}

void Script::deallocateScriptArray(void) {
    free(scriptArray);
    scriptArray = NULL;
}

void Script::deallocateNewScriptArray(void) {
    free(newScriptArray);
    newScriptArray = NULL;
}

