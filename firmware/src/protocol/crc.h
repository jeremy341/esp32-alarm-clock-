#pragma once

#include <stddef.h>
#include <stdint.h>

uint16_t crc16CcittUpdate(uint16_t crc, uint8_t value);
uint16_t crc16Ccitt(const uint8_t* data, size_t length);
