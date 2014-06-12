#include "openhive.h"
#include "types.h"
#include "Logging.h"
#include "tcp.h"
//#include "wdt.h"

// TCP handle used for Logging
TCP* tcpHandle;

void OpenHive::setup()
{
	InitLogging();
	LOG(LOG_PRINT,1,"OpenHive Setup");
	tcpHandle = new TCP();
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


    // check the uart port for data
    if ((millis() - mainState.lastEventImport) > IMPORT_PERIOD) {

        // TODO !
        tcpHandle->import();
        mainState.lastEventImport = millis();
    }

    // the actual execution
    if (transportState.gotScript) {
        if ((millis() - mainState.lastEventExecute) >= EXECUTE_PERIOD) {

            // led heartbit
            toggleled();

            // execute code in one iteration
            executeCode();

            // if needed send back the signal value
            tcpHandle->sendSignalValue();

            mainState.lastEventExecute = millis();
            // increment the global tick
            mainState.tick++;
        }
    }

    // ask for script when a new version has been seen
    if (transportState.askForFile == 1) {
        if ((millis() - mainState.lastEventAskFile) >= ASK_FOR_SCRIPT_PERIOD) {
            // ask for a script only when a new script has been seen
            if (tcpHandle->isScriptVerLarger(mainState.newScriptVer, mainState.scriptVer)) {
                LOG(LOG_TRANSP,2,"Asking for script: %d",mainState.newScriptVer);
                tcpHandle->askForFile();
            }
            mainState.lastEventAskFile = millis();
        }
    }

    // beacon
    if ((millis() - mainState.lastEventBeacon) >= BEACON_PERIOD) {
        // beacon only if there's a script on the node
        if (mainState.scriptVer > 0) {
            tcpHandle->beacon();
        }
        mainState.lastEventBeacon = millis();
    }

}

void OpenHive::executeCode(void) {

}

void OpenHive::toggleled(void) {
  ledstate = 1 - ledstate;
  digitalWrite(blinkingLed, ledstate);
}

