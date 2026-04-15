#pragma once

#include <Arduino.h>

#include "../protocol/protocol.h"

class SerialReceiver {
public:
  void begin(uint32_t baudRate);
  void update();

  bool hasPacket() const;
  const serial_protocol::Packet& packet() const;
  void consumePacket();

private:
  serial_protocol::PacketParser parser_;
};
