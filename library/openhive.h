#ifndef __openhive_h
#define __openhive_h

#include "Logging.h"

#define blinkingLed 13

class OpenHive {
private:
	// the global state of the application
	mainState_t mainState;

	// the state of the transport protocol
	transportState_t transportState;

	// the pin state info
	pinState_t pinState;

    // the state of the blinking LED
	uint8_t ledstate;
protected:
public:
	void setup(void);
	void loop(void);
	void executeCode(void);
	void toggleled(void);
};

#endif
