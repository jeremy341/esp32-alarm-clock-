#include <ArduinoJson.h>

#include "../../../protocol/constants.h"
#include "../../../protocol/preset_format.h"
#include "../core/alarm.h"
#include "../ui/presets.h"

namespace storage {
bool readTextFile(const char* path, String& output);
bool writeTextFile(const char* path, const String& content);
}  // namespace storage

namespace storage {

bool loadPresetSelection(ui::PresetSelection& selection) {
  String content;
  if (!readTextFile(alarm_clock_protocol::kConfigFilePath, content)) {
    return false;
  }

  StaticJsonDocument<256> document;
  if (deserializeJson(document, content)) {
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

bool savePresetSelection(const ui::PresetSelection& selection) {
  StaticJsonDocument<256> document;
  document[alarm_clock_format::kFieldVersion] = alarm_clock_protocol::kVersion;
  document[alarm_clock_format::kFieldLayout] = selection.layoutId;
  document[alarm_clock_format::kFieldTheme] = selection.themeId;
  document[alarm_clock_format::kFieldClockStyle] = selection.clockStyleId;

  String content;
  serializeJson(document, content);
  return writeTextFile(alarm_clock_protocol::kConfigFilePath, content);
}

bool loadAlarmSchedule(alarm_system::Schedule& schedule) {
  String content;
  if (!readTextFile(alarm_clock_protocol::kAlarmFilePath, content)) {
    return false;
  }

  StaticJsonDocument<128> document;
  if (deserializeJson(document, content)) {
    return false;
  }

  schedule.enabled = document["enabled"] | false;
  schedule.hour = document["hour"] | schedule.hour;
  schedule.minute = document["minute"] | schedule.minute;
  schedule.mode = static_cast<alarm_system::Mode>(document["mode"] | static_cast<uint8_t>(schedule.mode));
  return true;
}

bool saveAlarmSchedule(const alarm_system::Schedule& schedule) {
  StaticJsonDocument<128> document;
  document["enabled"] = schedule.enabled;
  document["hour"] = schedule.hour;
  document["minute"] = schedule.minute;
  document["mode"] = static_cast<uint8_t>(schedule.mode);

  String content;
  serializeJson(document, content);
  return writeTextFile(alarm_clock_protocol::kAlarmFilePath, content);
}

}  // namespace storage
