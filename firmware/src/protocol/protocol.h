#pragma once

#include <Arduino.h>

#include "../../../protocol/constants.h"

namespace serial_protocol {

struct Packet {
  uint8_t type = 0;
  uint16_t payloadLength = 0;
  char payload[alarm_clock_protocol::kMaxJsonPayloadBytes + 1] = {};
};

class PacketParser {
public:
  PacketParser();

  void reset();
  bool push(uint8_t value);
  bool hasPacket() const;
  const Packet& packet() const;
  void consumePacket();
  const char* lastError() const;

private:
  enum class State : uint8_t {
    WaitMagic0,
    WaitMagic1,
    ReadVersion,
    ReadType,
    ReadLength0,
    ReadLength1,
    ReadPayload,
    ReadCrc0,
    ReadCrc1
  };

  void fail(const char* error);

  State state_;
  Packet packet_;
  uint16_t payloadIndex_;
  uint16_t expectedLength_;
  uint16_t runningCrc_;
  uint16_t receivedCrc_;
  bool ready_;
  const char* lastError_;
};

}  // namespace serial_protocol
