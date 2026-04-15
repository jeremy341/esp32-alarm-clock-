export function encodePresetSelection(selection) {
  const payload = {
    version: 1,
    layout: selection.layout,
    theme: selection.theme,
    clockStyle: selection.clockStyle,
  };

  return new TextEncoder().encode(JSON.stringify(payload));
}
