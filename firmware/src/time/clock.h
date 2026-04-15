#pragma once

#include <Arduino.h>

struct ClockSnapshot {
  uint16_t year = 0;
  uint8_t month = 0;
  uint8_t day = 0;
  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t second = 0;
  bool valid = false;
  bool rtcAvailable = false;
};

class ClockService {
public:
  bool begin();
  bool update();
  const ClockSnapshot& snapshot() const;

private:
  bool refreshSnapshot(bool force);

  ClockSnapshot snapshot_;
  bool rtcAvailable_ = false;
  bool initialized_ = false;
  uint32_t lastPollMs_ = 0;
  uint32_t fallbackStartMs_ = 0;
};
