#include <ArduinoJson.h>

#include "../../../protocol/constants.h"
#include "../../../protocol/preset_format.h"
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

}  // namespace storage
