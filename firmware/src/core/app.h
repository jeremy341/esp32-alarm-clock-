#pragma once

#include "../display/display.h"
#include "../serial/receiver.h"
#include "../time/clock.h"
#include "../ui/presets.h"

class App {
public:
  void begin();
  void loop();

private:
  void applySelection(const ui::PresetSelection& selection, bool persistSelection);
  void handlePacket(const serial_protocol::Packet& packet);

  ClockService clock_;
  DisplayManager display_;
  SerialReceiver receiver_;
  ui::PresetSelection selection_;
  bool ready_ = false;
};
