#include "openhive.h"

// instantiate OpenHive class
OpenHive* oh = new OpenHive();

void setup() {
  // init the serial ports
  Serial.begin( 9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  // run OpenHive Arduino interpreter
  oh->run();
}

