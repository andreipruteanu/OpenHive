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
#include <malloc.h>
//#include "Actuator.h"
//#include "wdt.h"

void OpenHive::setup()
{
	InitLogging();
	LOG(LOG_PRINT,1,"OpenHive Setup");
    script     = new Script(&mainState);
    tcpHandler = new TCP(&mainState,script);
    
    ledstate   = 0;
}

void OpenHive::loop()
{
	LOG(LOG_PRINT,1,"OpenHive Loop");

    // restart WDT
    //wdt_restart(WDT);

    // activate the servo motors
    //if ((millis() - mainState.lastServoEvent)>SERVO_UPDATE_PERIOD) {
    //  for (uint8_t i=0; i< servoState.ServoCount; i++) {
    //    servoWrite(i, servoState.value[i]);
    //  }
    //  mainState.lastServoEvent = millis();
    //}

    // check lifecycle actions to be executed
    switch (mainState.lifecycleAction) {
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
    mainState.lifecycleAction = LA_NONE;

    // check the uart port for data
    if ((millis() - mainState.lastEventImport) > IMPORT_PERIOD) {

        // TODO !
        tcpHandler->import();
        mainState.lastEventImport = millis();
    }

    // the actual execution
    if ((tcpHandler->getTCP_State()).gotScript) {
        if ((millis() - mainState.lastEventExecute) >= EXECUTE_PERIOD) {

            // led heartbit
            toggleled();

            // execute code in one iteration
            executeCode();

            // if needed send back the signal value
            tcpHandler->sendSignalValue();

            mainState.lastEventExecute = millis();
            // increment the global tick
            mainState.tick++;
        }
    }

    // ask for script when a new version has been seen
    if ((tcpHandler->getTCP_State()).askForScript == 1) {
        if ((millis() - mainState.lastEventAskScript) >= ASK_FOR_SCRIPT_PERIOD) {
            // ask for a script only when a new script has been seen
            if (tcpHandler->isScriptVerLarger(mainState.newScriptVer, mainState.scriptVer)) {
                LOG(LOG_TRANSP,2,"Asking for script: %d",mainState.newScriptVer);
                tcpHandler->askForScript();
            }
            mainState.lastEventAskScript = millis();
        }
    }

    // beacon
    if ((millis() - mainState.lastEventBeacon) >= BEACON_PERIOD) {
        // beacon only if there's a script on the node
        if (mainState.scriptVer > 0) {
            tcpHandler->beacon();
        }
        mainState.lastEventBeacon = millis();
    }

}

void OpenHive::executeCode(void) {
    uint16_t blockNo;

    if ((tcpHandler->getTCP_State()).gotScript) {

        LOG(LOG_MAIN, 1, "tick %d start. ver: %d, rel: %d, MT: %d, DT: %d", mainState.tick, mainState.scriptVer, mainState.releaseLevel, mainState.maxExecutionTime, (mainState.maxExecutionTime * 1000 - millis()));
        LOG(LOG_MAIN, 2, "#blocks: %d", script->getBlockCount());
        
        for (uint16_t i=0; i < script->getBlockCount(); ++i) {  
            
            LOG(LOG_MAIN,  2,"start. blck nr:%d type:%d stateType:%d",i, (script->getBlockTypes())[i], (script->getBlockStateTypes())[i]);
            
            if ((script->getBlockStateTypes())[i] == BT_ALGO) {
                LOG(LOG_MAIN, 2,"algo exec");
                ((script->getBlocks())[i]).in(i);
                ((script->getBlocks())[i]).step(i);
                ((script->getBlocks())[i]).out(i);
            } else {
                LOG(LOG_MAIN, 2,"nonstate exec");
                ((script->getBlocks())[i]).out(i);
            }

            LOG(LOG_MAIN,2,"stop. block exec.");
        }
        
        tcpHandler->exportState();
        
        LOG(LOG_MAIN,  1, "tick %d end. code ver: %d", mainState.tick, mainState.scriptVer);
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
                (script->getBlocks())[i].deal(i);
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
        initMainState();
        
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
    
    for (uint16_t j=0; j<(script->getBlockCount()); ++j) {
        LOG(LOG_SCRIPT, 3, "block:%d/%d blocktype:%d", j, script->getBlockCount(), (script->getBlockStateTypes())[j]);

        uint8_t blockName = (script->getBlockStateTypes())[j];

        // TO DO: re-implement '=' operator
        //(script->getBlocks())[j] = Block::makeBlock(&mainState, blockName, j);
    }
}

void OpenHive::resetNode(void) {
    void(*SoftwareReset)(void) = 0;
}

void OpenHive::showStatus(void) {
    //struct mallinfo mem;
    //mem = mallinfo();
    LOG(LOG_PRINT,1,"STATUS: Firmware version: %ld", mainState.firmVer);
    LOG(LOG_PRINT,1,"STATUS: New script version: %ld", mainState.newScriptVer);    
    LOG(LOG_PRINT,1,"STATUS: Script version: %ld", mainState.scriptVer);
    LOG(LOG_PRINT,1,"STATUS: tick: %ld", mainState.tick);
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

void OpenHive::initMainState(void)
{

    // define the max debug level: 0 for "release" mode
    mainState.DEBUG_LEVEL = 255;

    // elapsed time
    mainState.eventImport         = 0;
    mainState.lastEventExecute    = 0;
    mainState.lastEventImport     = 0;
    mainState.lastEventAskScript  = 0;
    mainState.lastEventBeacon     = 0;
    mainState.scriptVer           = 0;
    mainState.newScriptVer        = 0;
    mainState.lastServoEvent      = 0;

    // the current tick
    mainState.tick = 0;

    // set the mem alloc flag to false initially
    mainState.memAlloc = 0;

    // set the firmware version
    mainState.firmVer = FIRMWARE_VERSION;

    // set the maximum execution time
    mainState.maxExecutionTime = MAX_EXECUTION_TIME;

    // set the release level
    mainState.releaseLevel = RELEASE_LEVEL;

    // initialize the reporting signal value
    mainState.reportSignalId[0] = -1;
    mainState.reportSignalId[1] = -1;
    mainState.reportSignalId[2] = -1;
    mainState.reportSignalId[3] = -1;
    mainState.reportSignalIdPort = PORTDEBUG;

}
