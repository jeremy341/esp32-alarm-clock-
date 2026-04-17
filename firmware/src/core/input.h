#pragma once

#include <Arduino.h>

namespace input {

enum class Button {
  Menu = 0,
  Up = 1,
  Down = 2,
};

class ButtonReader {
public:
  void begin();
  void update();
  bool pressed(Button button);

private:
  static constexpr uint8_t kButtonCount = 3;
  static constexpr uint8_t kMenuPin = 32;
  static constexpr uint8_t kUpPin = 33;
  static constexpr uint8_t kDownPin = 27;
  static constexpr unsigned long kDebounceMs = 50UL;

  const uint8_t pins_[kButtonCount] = {kMenuPin, kUpPin, kDownPin};
  bool stableState_[kButtonCount] = {true, true, true};
  bool lastRawState_[kButtonCount] = {true, true, true};
  unsigned long lastDebounceMs_[kButtonCount] = {0, 0, 0};
  bool event_[kButtonCount] = {false, false, false};
};

}  // namespace input
