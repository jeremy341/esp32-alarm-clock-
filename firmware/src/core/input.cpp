#include "input.h"

namespace input {

void ButtonReader::begin() {
  for (uint8_t index = 0; index < kButtonCount; ++index) {
    pinMode(pins_[index], INPUT_PULLUP);
    stableState_[index] = true;
    lastRawState_[index] = true;
    lastDebounceMs_[index] = millis();
    event_[index] = false;
  }
}

void ButtonReader::update() {
  const unsigned long now = millis();

  for (uint8_t index = 0; index < kButtonCount; ++index) {
    const bool raw = digitalRead(pins_[index]);

    if (raw != lastRawState_[index]) {
      lastDebounceMs_[index] = now;
      lastRawState_[index] = raw;
    }

    if ((now - lastDebounceMs_[index]) >= kDebounceMs && stableState_[index] != raw) {
      stableState_[index] = raw;
      if (!stableState_[index]) {
        event_[index] = true;
      }
    }
  }
}

bool ButtonReader::pressed(Button button) {
  const uint8_t index = static_cast<uint8_t>(button);
  if (index >= kButtonCount) {
    return false;
  }

  const bool wasPressed = event_[index];
  event_[index] = false;
  return wasPressed;
}

}  // namespace input
