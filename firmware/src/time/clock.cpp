#include "clock.h"

#include <RTClib.h>
#include <Wire.h>

namespace {

RTC_DS3231 rtc;
DateTime fallbackBase(__DATE__, __TIME__);

}  // namespace

bool ClockService::begin() {
  Wire.begin();
  rtcAvailable_ = rtc.begin();

  if (rtcAvailable_ && rtc.lostPower()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }

  fallbackStartMs_ = millis();
  initialized_ = true;
  return refreshSnapshot(true);
}

bool ClockService::update() {
  if (!initialized_) {
    return false;
  }

  const uint32_t nowMs = millis();
  if ((nowMs - lastPollMs_) < 200U) {
    return false;
  }

  lastPollMs_ = nowMs;
  return refreshSnapshot(false);
}

const ClockSnapshot& ClockService::snapshot() const {
  return snapshot_;
}

bool ClockService::refreshSnapshot(bool force) {
  DateTime now = rtcAvailable_
                     ? rtc.now()
                     : fallbackBase + TimeSpan(static_cast<int32_t>((millis() - fallbackStartMs_) / 1000U));

  const bool changed = force || now.second() != snapshot_.second ||
                       now.minute() != snapshot_.minute ||
                       now.hour() != snapshot_.hour ||
                       now.day() != snapshot_.day ||
                       now.month() != snapshot_.month ||
                       now.year() != snapshot_.year;

  snapshot_.year = now.year();
  snapshot_.month = now.month();
  snapshot_.day = now.day();
  snapshot_.hour = now.hour();
  snapshot_.minute = now.minute();
  snapshot_.second = now.second();
  snapshot_.valid = true;
  snapshot_.rtcAvailable = rtcAvailable_;
  return changed;
}
