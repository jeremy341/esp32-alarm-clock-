#include "../display/display.h"
#include "../core/alarm.h"

#include <stdio.h>
#include <string.h>

namespace {

const char* kWeekdays[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const char* kMonths[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                         "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

uint8_t weekdayIndex(const ClockSnapshot& snapshot) {
  uint16_t year = snapshot.year;
  uint8_t month = snapshot.month;

  if (month < 3) {
    month += 12;
    year -= 1;
  }

  const uint16_t yearOfCentury = year % 100;
  const uint16_t zeroBasedCentury = year / 100;
  const uint16_t dayOfWeek = (snapshot.day + ((13 * (month + 1)) / 5) + yearOfCentury +
                              (yearOfCentury / 4) + (zeroBasedCentury / 4) +
                              (5 * zeroBasedCentury)) %
                             7;

  return static_cast<uint8_t>((dayOfWeek + 6) % 7);
}

void formatPrimaryTime(const ui::ResolvedPreset& preset, const ClockSnapshot& snapshot,
                       bool colonVisible, char* buffer, size_t bufferSize) {
  uint8_t hour = snapshot.hour;

  if (!preset.clockStyle->hour24) {
    hour = static_cast<uint8_t>(hour % 12);
    if (hour == 0) {
      hour = 12;
    }
  }

  snprintf(buffer, bufferSize, "%02u%c%02u", hour, colonVisible ? ':' : ' ', snapshot.minute);
}

void formatSecondaryText(const ui::ResolvedPreset& preset, const ClockSnapshot& snapshot,
                         char* buffer, size_t bufferSize) {
  if (!preset.clockStyle->hour24) {
    snprintf(buffer, bufferSize, "%s %02u", snapshot.hour < 12 ? "AM" : "PM", snapshot.second);
    return;
  }

  snprintf(buffer, bufferSize, "%02u", snapshot.second);
}

void formatDateText(const ClockSnapshot& snapshot, char* buffer, size_t bufferSize) {
  snprintf(buffer, bufferSize, "%s %02u %s %04u",
           kWeekdays[weekdayIndex(snapshot)],
           snapshot.day,
           kMonths[snapshot.month - 1],
           snapshot.year);
}

}  // namespace

ClockRenderModel buildClockRenderModel(const ui::ResolvedPreset& preset, const ClockSnapshot& snapshot,
                                          const alarm_system::Schedule* alarm,
                                          bool alarmRinging) {
  ClockRenderModel model{};

  if (preset.clockStyle == nullptr || preset.layout == nullptr || !snapshot.valid) {
    snprintf(model.primary, sizeof(model.primary), "--:--");
    return model;
  }

  const bool colonVisible = !preset.clockStyle->blinkColon || ((snapshot.second % 2U) == 0U);
  formatPrimaryTime(preset, snapshot, colonVisible, model.primary, sizeof(model.primary));
  formatSecondaryText(preset, snapshot, model.secondary, sizeof(model.secondary));
  formatDateText(snapshot, model.date, sizeof(model.date));

  model.showDate = preset.clockStyle->showDate;
  model.showSecondary = preset.clockStyle->splitSeconds ||
                        preset.layout->hasSecondsPanel ||
                        !preset.clockStyle->hour24;
  model.colonVisible = colonVisible;
  model.primaryFont = preset.clockStyle->primaryFont;
  model.secondaryFont = preset.clockStyle->secondaryFont;

  if (alarm != nullptr && alarm->enabled) {
    model.showAlarmStatus = true;
    if (alarmRinging) {
      snprintf(model.alarmStatus, sizeof(model.alarmStatus), "ALARM %02u:%02u", alarm->hour, alarm->minute);
      model.alarmRinging = true;
    } else {
      snprintf(model.alarmStatus, sizeof(model.alarmStatus), "Alarm %02u:%02u %s",
               alarm->hour, alarm->minute, alarm_system::modeShortLabel(alarm->mode));
    }
  }

  return model;
}
