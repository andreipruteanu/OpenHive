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

#include "openhive.h"
#include "types.h"
#include "Logging.h"
#include "tcp.h"
#include "block.h"
#include "block_factory.h"
#include "block_gradient.h"
#include <malloc.h>
//#include "Actuator.h"
//#include "wdt.h"

void OpenHive::setup()
{
	InitLogging();
	LOG(LOG_PRINT,1,"OpenHive Setup");

    // create objects
    script       = new Script(&runtimeState);
    tcpHandler   = new TCP(&runtimeState,script);
    blockFactory = new BlockFactory();
    
    ledstate   = 0;
}

void OpenHive::loop()
{
	LOG(LOG_PRINT,1,"OpenHive Loop");

    // restart WDT
    //wdt_restart(WDT);

    // activate the servo motors
    //if ((millis() - runtimeState.lastServoEvent)>SERVO_UPDATE_PERIOD) {
    //  for (uint8_t i=0; i< servoState.ServoCount; i++) {
    //    servoWrite(i, servoState.value[i]);
    //  }
    //  runtimeState.lastServoEvent = millis();
    //}

    // check lifecycle actions to be executed
    switch (runtimeState.lifecycleAction) {
        case LA_DEALLOCATE:
            deallocate();
            break;
        case LA_RESET_NODE:
            resetNode();
            break;
        case LA_SHOW_STATUS:
            showStatus();
            break;
        default:
            break;
    }

    // return to lifecycle "none" action
    runtimeState.lifecycleAction = LA_NONE;

    // check the uart port for data
    if ((millis() - runtimeState.lastEventImport) > IMPORT_PERIOD) {

        // TODO !
        tcpHandler->import();
        runtimeState.lastEventImport = millis();
    }

    // the actual execution
    if ((tcpHandler->getTCP_State()).gotScript) {
        if ((millis() - runtimeState.lastEventExecute) >= EXECUTE_PERIOD) {

            // led heartbit
            toggleled();

            // execute code in one iteration
            executeCode();

            // if needed send back the signal value
            tcpHandler->sendSignalValue();

            runtimeState.lastEventExecute = millis();
            // increment the global tick
            runtimeState.tick++;
        }
    }

    // ask for script when a new version has been seen
    if ((tcpHandler->getTCP_State()).askForScript == 1) {
        if ((millis() - runtimeState.lastEventAskScript) >= ASK_FOR_SCRIPT_PERIOD) {
            // ask for a script only when a new script has been seen
            if (tcpHandler->isScriptVerLarger(runtimeState.newScriptVer, runtimeState.scriptVer)) {
                LOG(LOG_TRANSP,2,"Asking for script: %d",runtimeState.newScriptVer);
                tcpHandler->askForScript();
            }
            runtimeState.lastEventAskScript = millis();
        }
    }

    // beacon
    if ((millis() - runtimeState.lastEventBeacon) >= BEACON_PERIOD) {
        // beacon only if there's a script on the node
        if (runtimeState.scriptVer > 0) {
            tcpHandler->beacon();
        }
        runtimeState.lastEventBeacon = millis();
    }

}

void OpenHive::executeCode(void) {
    uint16_t blockNo;

    if ((tcpHandler->getTCP_State()).gotScript) {

        LOG(LOG_MAIN, 1, "tick %d start. ver: %d, rel: %d, MT: %d, DT: %d", runtimeState.tick, runtimeState.scriptVer, runtimeState.releaseLevel, runtimeState.maxExecutionTime, (runtimeState.maxExecutionTime * 1000 - millis()));
        LOG(LOG_MAIN, 2, "#blocks: %d", script->getBlockCount());
        
        for (uint16_t i=0; i < script->getBlockCount(); ++i) {  
            
            LOG(LOG_MAIN,  2,"start. blck nr:%d type:%d stateType:%d",i, (script->getBlockTypes())[i], (script->getBlockStateTypes())[i]);
            
            if ((script->getBlockStateTypes())[i] == BT_ALGO) {
                LOG(LOG_MAIN, 2,"algo exec");
                (*((script->getBlocks())[i])).in();
                (*((script->getBlocks())[i])).step();
                (*((script->getBlocks())[i])).out();
            } else {
                LOG(LOG_MAIN, 2,"nonstate exec");
                (*((script->getBlocks())[i])).out();
            }

            LOG(LOG_MAIN,2,"stop. block exec.");
        }
        
        tcpHandler->exportState();
        
        LOG(LOG_MAIN,  1, "tick %d end. code ver: %d", runtimeState.tick, runtimeState.scriptVer);
    }
}

void OpenHive::toggleled(void) {
  ledstate = 1 - ledstate;
  digitalWrite(blinkingLed, ledstate);
}

void OpenHive::deallocate(void) {

    if ((tcpHandler->getTCP_State()).gotScript) {
        LOG(LOG_DEAL,1,"Started de-allocating");

        // set the gotScript flag to false
        (tcpHandler->getTCP_State()).gotScript = 0;       

        uint16_t i=0;

        // deallocate the state of the blocks
        for (i=0; i < script->getBlockCount(); ++i) {  
            if ((script->getBlockStateTypes())[i] != BT_SIMPLE) {
                LOG(LOG_DEAL,2,"deal block %d",i);
                (*(script->getBlocks())[i]).deallocate();
            }
        }

        // de-allocate the signals
        script->deallocateSignals();

        // de-allocate input and output mapping in the script
        script->deallocatePorts();

        // de-allocate blocks 
        script->deallocateBlocks();

        // de-allocate block types
        script->deallocateBlockTypes();

        // de-allocate script
        script->deallocateScriptArray();

        // de-allocate new script
        script->deallocateNewScriptArray();
        
        // initialise the main state
        initruntimeState();
        
        // initialise the transport protocol state
        tcpHandler->initTCP();

        // initialize the blocks that have state
        initBlocks();

        // print malloc stats: - not supported on 8 bit controllers !
        //struct mallinfo mem;
        //mem = mallinfo();

        //LOG(LOG_DEAL,1,"Finished de-allocating. Malloc stats:");
        //LOG(LOG_DEAL,2,"Total free space (bytes): %d",mem.fordblks);

    } else {
        LOG(LOG_DEAL,1,"No script available. Nothing to de-allocate !");
    }
}

void OpenHive::initBlocks(void) {
    // initialize the blocks that have state
    LOG(LOG_SCRIPT, 3, "# blocks: %d", script->getBlockCount());
    
    uint16_t blockCount = script->getBlockCount();
    Block** blocks      = script->getBlocks();

    // allocate the new script
    blocks = new Block*[blockCount];

    for (uint16_t j=0; j<blockCount; ++j) {
        LOG(LOG_SCRIPT, 3, "block:%d/%d blocktype:%d", j, blockCount, (script->getBlockStateTypes())[j]);

        uint8_t blockName = (script->getBlockStateTypes())[j];
        
        blocks[j] = blockFactory->makeBlock(&runtimeState, blockName, j);
    }
}

void OpenHive::resetNode(void) {
    void(*SoftwareReset)(void) = 0;
}

void OpenHive::showStatus(void) {
    //struct mallinfo mem;
    //mem = mallinfo();
    LOG(LOG_PRINT,1,"STATUS: Firmware version: %ld", runtimeState.firmVer);
    LOG(LOG_PRINT,1,"STATUS: New script version: %ld", runtimeState.newScriptVer);    
    LOG(LOG_PRINT,1,"STATUS: Script version: %ld", runtimeState.scriptVer);
    LOG(LOG_PRINT,1,"STATUS: tick: %ld", runtimeState.tick);
    //LOG(LOG_PRINT,1,"STATUS: Free mem: %d, alloc mem: %d", mem.fordblks, mem.uordblks);

    if ((tcpHandler->getTCP_State()).gotScript) {
        // read number of blocks - two bytes
        LOG(LOG_PRINT, 1, "STATUS: nr blocks %d", script->getBlockCount());
  
        // read number of signals - two bytes
        LOG(LOG_PRINT, 1, "STATUS: nr signals %d", script->getSignalCount());
  
        // read number of init signals
        LOG(LOG_PRINT, 1, "STATUS: nr init signals %d", script->getInitSigsCount());
  
        // read the blocks themselves
        for (uint16_t i=0; i < script->getBlockCount(); ++i) {
          LOG(LOG_PRINT, 1, "STATUS: signal types[%d] = %d", i, (script->getBlockTypes())[i]);
        }
        
        for (uint16_t i=0; i < script->getBlockCount(); ++i) {
    
          const blockinfo_t* binfo = script->getBlockInfo((script->getBlockTypes())[i]);
          LOG(LOG_PRINT, 1, "STATUS: block %d: ninputs:%d noutputs:%d", i, binfo->nin, binfo->nout);
    
          for (uint16_t j=0; j< binfo->nin; ++j) {
            LOG(LOG_PRINT, 1, "STATUS: input %d: %d", j, (script->getPorts())[i].in[j]);
          }
          for (uint16_t j=0; j< binfo->nout; ++j) {
            LOG(LOG_PRINT, 1, "STATUS: output %d: %d", j, (script->getPorts())[i].out[j]);
          }
        }

    } else {
        LOG(LOG_PRINT,1,"STATUS: no script allocated.");
    }
}

void OpenHive::initruntimeState(void)
{

    // define the max debug level: 0 for "release" mode
    runtimeState.DEBUG_LEVEL = 255;

    // elapsed time
    runtimeState.eventImport         = 0;
    runtimeState.lastEventExecute    = 0;
    runtimeState.lastEventImport     = 0;
    runtimeState.lastEventAskScript  = 0;
    runtimeState.lastEventBeacon     = 0;
    runtimeState.scriptVer           = 0;
    runtimeState.newScriptVer        = 0;
    runtimeState.lastServoEvent      = 0;

    // the current tick
    runtimeState.tick = 0;

    // set the mem alloc flag to false initially
    runtimeState.memAlloc = 0;

    // set the firmware version
    runtimeState.firmVer = FIRMWARE_VERSION;

    // set the maximum execution time
    runtimeState.maxExecutionTime = MAX_EXECUTION_TIME;

    // set the release level
    runtimeState.releaseLevel = RELEASE_LEVEL;

    // initialize the reporting signal value
    runtimeState.reportSignalId[0] = -1;
    runtimeState.reportSignalId[1] = -1;
    runtimeState.reportSignalId[2] = -1;
    runtimeState.reportSignalId[3] = -1;
    runtimeState.reportSignalIdPort = PORTDEBUG;

}
