import { buildPresetPacket } from "../export/packet_builder.js";

export async function uploadPreset(client, selection) {
  const packet = buildPresetPacket(selection);
  await client.send(packet);
}
