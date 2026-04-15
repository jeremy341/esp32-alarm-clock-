#pragma once

#include <stddef.h>
#include <stdint.h>

namespace alarm_clock_protocol {

static constexpr uint8_t kMagic0 = 0xAC;
static constexpr uint8_t kMagic1 = 0xDC;
static constexpr uint8_t kVersion = 1;
static constexpr uint8_t kPacketTypePresetConfig = 0x01;

static constexpr size_t kMaxJsonPayloadBytes = 384;
static constexpr size_t kPacketOverheadBytes = 8;
static constexpr size_t kMaxPacketBytes = kMaxJsonPayloadBytes + kPacketOverheadBytes;

static constexpr char kConfigFilePath[] = "/preset.json";
static constexpr char kDeviceName[] = "ESP32 Alarm Clock";

}  // namespace alarm_clock_protocol
