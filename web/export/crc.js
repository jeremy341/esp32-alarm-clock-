export function crc16Ccitt(bytes) {
  let crc = 0xffff;

  for (const byte of bytes) {
    crc ^= byte << 8;

    for (let bit = 0; bit < 8; bit += 1) {
      if ((crc & 0x8000) !== 0) {
        crc = ((crc << 1) ^ 0x1021) & 0xffff;
      } else {
        crc = (crc << 1) & 0xffff;
      }
    }
  }

  return crc;
}
