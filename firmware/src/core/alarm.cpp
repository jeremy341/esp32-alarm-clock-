#include "alarm.h"

#include <stdio.h>

namespace alarm_system {

const char* modeLabel(Mode mode) {
  switch (mode) {
    case Mode::Weekdays:
      return "Mon-Fri";
    case Mode::Weekend:
      return "Sat-Sun";
    case Mode::Everyday:
      return "Everyday";
    default:
      return "Unknown";
  }
}

const char* modeShortLabel(Mode mode) {
  switch (mode) {
    case Mode::Weekdays:
      return "M-F";
    case Mode::Weekend:
      return "S-S";
    case Mode::Everyday:
      return "Both";
    default:
      return "Err";
  }
}

uint8_t weekdayIndex(uint16_t year, uint8_t month, uint8_t day) {
  if (month < 3) {
    month += 12;
    year -= 1;
  }

  const uint16_t yearOfCentury = year % 100;
  const uint16_t century = year / 100;
  const uint16_t w = (day + (13 * (month + 1)) / 5 + yearOfCentury + yearOfCentury / 4 + century / 4 + 5 * century) % 7;
  return static_cast<uint8_t>((w + 6) % 7);
}

bool isActiveToday(const Schedule& schedule, const ClockSnapshot& snapshot) {
  if (!snapshot.valid) {
    return false;
  }

  const uint8_t weekday = weekdayIndex(snapshot.year, snapshot.month, snapshot.day);
  switch (schedule.mode) {
    case Mode::Weekdays:
      return weekday >= 1 && weekday <= 5;
    case Mode::Weekend:
      return weekday == 0 || weekday == 6;
    case Mode::Everyday:
      return true;
    default:
      return false;
  }
}

bool isTimeMatch(const Schedule& schedule, const ClockSnapshot& snapshot) {
  return snapshot.valid && snapshot.hour == schedule.hour && snapshot.minute == schedule.minute;
}

void formatClockTime(uint8_t hour, uint8_t minute, char* output, size_t size) {
  snprintf(output, size, "%02u:%02u", hour, minute);
}

}  // namespace alarm_system
