#include "app.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#include "../../../protocol/constants.h"
#include "../../../protocol/preset_format.h"

namespace storage {
bool beginFileSystem();
bool loadPresetSelection(ui::PresetSelection& selection);
bool savePresetSelection(const ui::PresetSelection& selection);
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

  storage::beginFileSystem();
  storage::loadPresetSelection(selection_);
  ui::normalizeSelection(selection_);

  receiver_.begin(115200);
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

  if (receiver_.hasPacket()) {
    handlePacket(receiver_.packet());
    receiver_.consumePacket();
  }

  if (clock_.update() && ready_) {
    display_.updateClock(clock_.snapshot());
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
