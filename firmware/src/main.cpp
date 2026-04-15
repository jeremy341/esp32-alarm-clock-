#include <Arduino.h>

#include "core/app.h"

App app;

void setup() {
  app.begin();
}

void loop() {
  app.loop();
}
