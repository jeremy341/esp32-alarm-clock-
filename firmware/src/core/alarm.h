#pragma once

#include "../time/clock.h"
#include <stdint.h>

namespace alarm_system {

enum class Mode : uint8_t {
  Weekdays = 0,
  Weekend = 1,
  Everyday = 2,
};

struct Schedule {
  bool enabled = false;
  uint8_t hour = 7;
  uint8_t minute = 0;
  Mode mode = Mode::Weekdays;
};

const char* modeLabel(Mode mode);
const char* modeShortLabel(Mode mode);
uint8_t weekdayIndex(uint16_t year, uint8_t month, uint8_t day);
bool isActiveToday(const Schedule& schedule, const ClockSnapshot& snapshot);
bool isTimeMatch(const Schedule& schedule, const ClockSnapshot& snapshot);
void formatClockTime(uint8_t hour, uint8_t minute, char* output, size_t size);

}  // namespace alarm_system
