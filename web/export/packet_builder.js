import { crc16Ccitt } from "./crc.js";
import { encodePresetSelection } from "./encoder.js";

const MAGIC0 = 0xac;
const MAGIC1 = 0xdc;
const VERSION = 0x01;
const PACKET_TYPE_PRESET_CONFIG = 0x01;

export function buildPresetPacket(selection) {
  const payload = encodePresetSelection(selection);
  const header = new Uint8Array(4);
  header[0] = VERSION;
  header[1] = PACKET_TYPE_PRESET_CONFIG;
  header[2] = payload.length & 0xff;
  header[3] = (payload.length >> 8) & 0xff;

  const crcInput = new Uint8Array(header.length + payload.length);
  crcInput.set(header, 0);
  crcInput.set(payload, header.length);

  const crc = crc16Ccitt(crcInput);
  const packet = new Uint8Array(2 + crcInput.length + 2);
  packet[0] = MAGIC0;
  packet[1] = MAGIC1;
  packet.set(crcInput, 2);
  packet[packet.length - 2] = crc & 0xff;
  packet[packet.length - 1] = (crc >> 8) & 0xff;
  return packet;
}
