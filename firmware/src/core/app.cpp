#include "app.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#include "../../../protocol/constants.h"
#include "../../../protocol/preset_format.h"

namespace storage {
bool beginFileSystem();
bool loadPresetSelection(ui::PresetSelection& selection);
bool savePresetSelection(const ui::PresetSelection& selection);
bool loadAlarmSchedule(alarm_system::Schedule& schedule);
bool saveAlarmSchedule(const alarm_system::Schedule& schedule);
}  // namespace storage

namespace {

bool decodeSelectionPayload(const char* json, ui::PresetSelection& selection) {
  StaticJsonDocument<256> document;
  const DeserializationError error = deserializeJson(document, json);

  if (error) {
    return false;
  }

  ui::copyId(selection.layoutId, sizeof(selection.layoutId),
             document[alarm_clock_format::kFieldLayout] | "");
  ui::copyId(selection.themeId, sizeof(selection.themeId),
             document[alarm_clock_format::kFieldTheme] | "");
  ui::copyId(selection.clockStyleId, sizeof(selection.clockStyleId),
             document[alarm_clock_format::kFieldClockStyle] | "");
  return ui::normalizeSelection(selection);
}

}  // namespace

void App::begin() {
  selection_ = ui::defaultSelection();
  alarm_ = alarm_system::Schedule{};

  storage::beginFileSystem();
  storage::loadPresetSelection(selection_);
  storage::loadAlarmSchedule(alarm_);
  ui::normalizeSelection(selection_);

  receiver_.begin(115200);
  input_.begin();
  Serial.println();
  Serial.println(alarm_clock_protocol::kDeviceName);

  clock_.begin();
  display_.begin();
  applySelection(selection_, false);

  ready_ = true;
  Serial.println("System ready.");
}

void App::loop() {
  receiver_.update();
  input_.update();

  if (receiver_.hasPacket()) {
    handlePacket(receiver_.packet());
    receiver_.consumePacket();
  }

  const ClockSnapshot snapshot = clock_.snapshot();
  handleButtonInput(snapshot);

  if (clock_.update() && ready_) {
    processAlarm(snapshot);

    if (menuState_ == MenuState::Clock) {
      display_.updateClock(snapshot, false, &alarm_, alarmTriggered_);
    }
  }
}

void App::applySelection(const ui::PresetSelection& selection, bool persistSelection) {
  selection_ = selection;
  ui::normalizeSelection(selection_);

  if (persistSelection) {
    storage::savePresetSelection(selection_);
  }

  display_.setPreset(ui::resolve(selection_));
  display_.renderFull(clock_.snapshot());

  Serial.print("Applied layout=");
  Serial.print(selection_.layoutId);
  Serial.print(", theme=");
  Serial.print(selection_.themeId);
  Serial.print(", clockStyle=");
  Serial.println(selection_.clockStyleId);
}

void App::handleButtonInput(const ClockSnapshot& snapshot) {
  if (input_.pressed(input::Button::Menu)) {
    if (menuState_ == MenuState::Clock) {
      menuState_ = MenuState::SetAlarmHour;
      alarm_.enabled = true;
      renderAlarmMenu(snapshot);
      return;
    }

    if (menuState_ == MenuState::SetAlarmHour) {
      menuState_ = MenuState::SetAlarmMinute;
    } else if (menuState_ == MenuState::SetAlarmMinute) {
      menuState_ = MenuState::SetAlarmDays;
    } else if (menuState_ == MenuState::SetAlarmDays) {
      menuState_ = MenuState::Clock;
      saveAlarmSchedule();
      display_.renderFull(snapshot);
      return;
    }

    renderAlarmMenu(snapshot);
    return;
  }

  if (menuState_ != MenuState::Clock) {
    int delta = 0;
    if (input_.pressed(input::Button::Up)) {
      delta = 1;
    } else if (input_.pressed(input::Button::Down)) {
      delta = -1;
    }

    if (delta != 0) {
      if (menuState_ == MenuState::SetAlarmHour) {
        alarm_.hour = static_cast<uint8_t>((alarm_.hour + 24 + delta) % 24);
      } else if (menuState_ == MenuState::SetAlarmMinute) {
        alarm_.minute = static_cast<uint8_t>((alarm_.minute + 60 + delta) % 60);
      } else if (menuState_ == MenuState::SetAlarmDays) {
        const int mode = static_cast<int>(alarm_.mode);
        const int nextMode = (mode + 3 + delta) % 3;
        alarm_.mode = static_cast<alarm_system::Mode>(nextMode);
      }

      renderAlarmMenu(snapshot);
    }
  }
}

void App::renderAlarmMenu(const ClockSnapshot& snapshot) {
  char line1[32];
  char line2[32];
  char line3[32];

  snprintf(line1, sizeof(line1), "Time: %02u:%02u", alarm_.hour, alarm_.minute);
  snprintf(line2, sizeof(line2), "Days: %s", alarm_system::modeLabel(alarm_.mode));

  if (menuState_ == MenuState::SetAlarmHour) {
    snprintf(line3, sizeof(line3), "Menu=Next  Up/Down=Hour");
  } else if (menuState_ == MenuState::SetAlarmMinute) {
    snprintf(line3, sizeof(line3), "Menu=Next  Up/Down=Minute");
  } else {
    snprintf(line3, sizeof(line3), "Menu=Save  Up/Down=Mode");
  }

  if (alarm_.enabled) {
    display_.renderMenuScreen("Alarm Setup", line1, line2, line3);
  } else {
    display_.renderMenuScreen("Alarm Setup", "Alarm disabled", line2, line3);
  }
}

void App::saveAlarmSchedule() {
  if (storage::saveAlarmSchedule(alarm_)) {
    Serial.println("Alarm schedule saved.");
  } else {
    Serial.println("Failed to save alarm schedule.");
  }
}

void App::processAlarm(const ClockSnapshot& snapshot) {
  if (!alarm_.enabled || !alarm_system::isActiveToday(alarm_, snapshot) || !alarm_system::isTimeMatch(alarm_, snapshot)) {
    alarmTriggered_ = false;
    return;
  }

  if (snapshot.minute != lastAlarmMinute_ || snapshot.day != lastAlarmDay_) {
    alarmTriggered_ = true;
    lastAlarmMinute_ = snapshot.minute;
    lastAlarmDay_ = snapshot.day;
    Serial.println("Alarm triggered!");
  }
}

void App::handlePacket(const serial_protocol::Packet& packet) {
  if (packet.type != alarm_clock_protocol::kPacketTypePresetConfig) {
    Serial.println("Ignoring unsupported packet type.");
    return;
  }

  ui::PresetSelection incoming = selection_;
  if (!decodeSelectionPayload(packet.payload, incoming)) {
    Serial.println("Rejected malformed preset payload.");
    return;
  }

  applySelection(incoming, true);
}
