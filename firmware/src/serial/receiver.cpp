#include "receiver.h"

void SerialReceiver::begin(uint32_t baudRate) {
  Serial.begin(baudRate);
  delay(100);
}

void SerialReceiver::update() {
  while (Serial.available() > 0) {
    const uint8_t value = static_cast<uint8_t>(Serial.read());
    parser_.push(value);

    if (parser_.hasPacket()) {
      break;
    }
  }
}

bool SerialReceiver::hasPacket() const {
  return parser_.hasPacket();
}

const serial_protocol::Packet& SerialReceiver::packet() const {
  return parser_.packet();
}

void SerialReceiver::consumePacket() {
  parser_.consumePacket();
}
