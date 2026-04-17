#pragma once

#include "../display/display.h"
#include "../serial/receiver.h"
#include "../time/clock.h"
#include "../ui/presets.h"
#include "input.h"
#include "alarm.h"

class App {
public:
  void begin();
  void loop();

private:
  enum class MenuState {
    Clock,
    SetAlarmHour,
    SetAlarmMinute,
    SetAlarmDays,
  };

  void applySelection(const ui::PresetSelection& selection, bool persistSelection);
  void handlePacket(const serial_protocol::Packet& packet);
  void handleButtonInput(const ClockSnapshot& snapshot);
  void renderAlarmMenu(const ClockSnapshot& snapshot);
  void saveAlarmSchedule();
  void processAlarm(const ClockSnapshot& snapshot);

  ClockService clock_;
  DisplayManager display_;
  SerialReceiver receiver_;
  input::ButtonReader input_;
  alarm_system::Schedule alarm_;
  ui::PresetSelection selection_;
  MenuState menuState_ = MenuState::Clock;
  bool alarmTriggered_ = false;
  uint8_t lastAlarmDay_ = 255;
  uint8_t lastAlarmMinute_ = 255;
  bool ready_ = false;
};
