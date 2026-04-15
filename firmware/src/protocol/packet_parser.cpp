#include "protocol.h"

#include "crc.h"

namespace serial_protocol {

PacketParser::PacketParser() {
  reset();
}

void PacketParser::reset() {
  state_ = State::WaitMagic0;
  packet_ = Packet{};
  payloadIndex_ = 0;
  expectedLength_ = 0;
  runningCrc_ = 0xFFFFU;
  receivedCrc_ = 0;
  ready_ = false;
  lastError_ = "";
}

bool PacketParser::push(uint8_t value) {
  switch (state_) {
    case State::WaitMagic0:
      if (value == alarm_clock_protocol::kMagic0) {
        state_ = State::WaitMagic1;
      }
      return false;

    case State::WaitMagic1:
      if (value == alarm_clock_protocol::kMagic1) {
        state_ = State::ReadVersion;
        runningCrc_ = 0xFFFFU;
        payloadIndex_ = 0;
        expectedLength_ = 0;
        packet_ = Packet{};
        ready_ = false;
      } else {
        state_ = State::WaitMagic0;
      }
      return false;

    case State::ReadVersion:
      if (value != alarm_clock_protocol::kVersion) {
        fail("Protocol version mismatch");
        return false;
      }

      runningCrc_ = crc16CcittUpdate(runningCrc_, value);
      state_ = State::ReadType;
      return false;

    case State::ReadType:
      packet_.type = value;
      runningCrc_ = crc16CcittUpdate(runningCrc_, value);
      state_ = State::ReadLength0;
      return false;

    case State::ReadLength0:
      expectedLength_ = value;
      runningCrc_ = crc16CcittUpdate(runningCrc_, value);
      state_ = State::ReadLength1;
      return false;

    case State::ReadLength1:
      expectedLength_ |= static_cast<uint16_t>(value) << 8U;
      runningCrc_ = crc16CcittUpdate(runningCrc_, value);

      if (expectedLength_ > alarm_clock_protocol::kMaxJsonPayloadBytes) {
        fail("Payload too large");
        return false;
      }

      packet_.payloadLength = expectedLength_;
      state_ = expectedLength_ == 0 ? State::ReadCrc0 : State::ReadPayload;
      return false;

    case State::ReadPayload:
      packet_.payload[payloadIndex_++] = static_cast<char>(value);
      runningCrc_ = crc16CcittUpdate(runningCrc_, value);

      if (payloadIndex_ >= expectedLength_) {
        packet_.payload[payloadIndex_] = '\0';
        state_ = State::ReadCrc0;
      }
      return false;

    case State::ReadCrc0:
      receivedCrc_ = value;
      state_ = State::ReadCrc1;
      return false;

    case State::ReadCrc1:
      receivedCrc_ |= static_cast<uint16_t>(value) << 8U;
      if (receivedCrc_ != runningCrc_) {
        fail("CRC mismatch");
        return false;
      }

      ready_ = true;
      state_ = State::WaitMagic0;
      return true;
  }

  fail("Parser state error");
  return false;
}

bool PacketParser::hasPacket() const {
  return ready_;
}

const Packet& PacketParser::packet() const {
  return packet_;
}

void PacketParser::consumePacket() {
  ready_ = false;
}

const char* PacketParser::lastError() const {
  return lastError_;
}

void PacketParser::fail(const char* error) {
  lastError_ = error;
  state_ = State::WaitMagic0;
  payloadIndex_ = 0;
  expectedLength_ = 0;
  runningCrc_ = 0xFFFFU;
  receivedCrc_ = 0;
  ready_ = false;
}

}  // namespace serial_protocol
