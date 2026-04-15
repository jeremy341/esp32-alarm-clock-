#pragma once

#include <stddef.h>
#include <stdint.h>

#include "constants.h"

namespace alarm_clock_protocol {

struct PacketHeader {
  uint8_t version;
  uint8_t type;
  uint16_t payloadLength;
};

static constexpr size_t kPreambleBytes = 2;
static constexpr size_t kHeaderBytes = sizeof(PacketHeader);
static constexpr size_t kCrcBytes = 2;

}  // namespace alarm_clock_protocol
