#include "crc.h"

uint16_t crc16CcittUpdate(uint16_t crc, uint8_t value) {
  crc ^= static_cast<uint16_t>(value) << 8U;

  for (uint8_t bit = 0; bit < 8; ++bit) {
    if ((crc & 0x8000U) != 0U) {
      crc = static_cast<uint16_t>((crc << 1U) ^ 0x1021U);
    } else {
      crc = static_cast<uint16_t>(crc << 1U);
    }
  }

  return crc;
}

uint16_t crc16Ccitt(const uint8_t* data, size_t length) {
  uint16_t crc = 0xFFFFU;

  for (size_t index = 0; index < length; ++index) {
    crc = crc16CcittUpdate(crc, data[index]);
  }

  return crc;
}
