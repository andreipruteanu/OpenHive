#include "openhive.h"

OpenHive* oh = new OpenHive();

void setup() {
  // init the serial ports
  Serial.begin( 9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  
  // init open hive object
  oh->setup();
}

void loop() {
  // loop open hive object
  oh->loop();
}

