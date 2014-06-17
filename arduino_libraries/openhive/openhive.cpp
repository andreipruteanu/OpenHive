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
//#include "wdt.h"

void OpenHive::setup()
{
	InitLogging();
	LOG(LOG_PRINT,1,"OpenHive Setup");
	tcpHandler = new TCP(&mainState);
	ledstate = 0;
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
    if ((tcpHandler->getTCP_State()).askForFile == 1) {
        if ((millis() - mainState.lastEventAskFile) >= ASK_FOR_SCRIPT_PERIOD) {
            // ask for a script only when a new script has been seen
            if (tcpHandler->isScriptVerLarger(mainState.newScriptVer, mainState.scriptVer)) {
                LOG(LOG_TRANSP,2,"Asking for script: %d",mainState.newScriptVer);
                tcpHandler->askForFile();
            }
            mainState.lastEventAskFile = millis();
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
        LOG(LOG_MAIN, 2, "#blocks: %d", *script->nblocks);
    
        for (uint16_t i=0; i<*script->nblocks; ++i) {  
        
            LOG(LOG_MAIN,  2,"start. blck nr:%d type:%d stateType:%d",i, script->blockTypes[i], script->blockStateType[i]);
            
            if (script->blockStateType[i] == BT_ALGO) {
                LOG(LOG_MAIN, 2,"algo exec");
                script->blocks[i].in(i);
                script->blocks[i].step(i);
                script->blocks[i].out(i);
            } else {
                LOG(LOG_MAIN, 2,"nonstate exec");
                script->blocks[i].out(i);
            }

            LOG(LOG_MAIN,2,"stop. block exec.");
        }
      
        tcpHandler->exportState(script);
        
        LOG(LOG_MAIN,  1, "tick %d end. code ver: %d", mainState.tick, mainState.scriptVer);
    }
}

void OpenHive::toggleled(void) {
  ledstate = 1 - ledstate;
  digitalWrite(blinkingLed, ledstate);
}

void OpenHive::deallocate(void) {

}

void OpenHive::resetNode(void) {

}

void OpenHive::showStatus(void) {

}



