#include "../display/display.h"

namespace {

uint8_t clampChannel(int32_t value) {
  if (value < 0) {
    return 0;
  }

  if (value > 255) {
    return 255;
  }

  return static_cast<uint8_t>(value);
}

uint16_t adjustColor(uint16_t color, uint8_t brightness, uint8_t contrast) {
  int32_t red = ((color >> 11) & 0x1FU) * 255 / 31;
  int32_t green = ((color >> 5) & 0x3FU) * 255 / 63;
  int32_t blue = (color & 0x1FU) * 255 / 31;

  red = (((red - 128) * contrast) / 255) + 128;
  green = (((green - 128) * contrast) / 255) + 128;
  blue = (((blue - 128) * contrast) / 255) + 128;

  red = (red * brightness) / 255;
  green = (green * brightness) / 255;
  blue = (blue * brightness) / 255;

  return ui::rgb565(clampChannel(red), clampChannel(green), clampChannel(blue));
}

}  // namespace

ThemePalette buildThemePalette(const ui::ResolvedPreset& preset) {
  ThemePalette palette{};

  if (preset.theme == nullptr) {
    return palette;
  }

  palette.background = adjustColor(preset.theme->background, preset.theme->brightness, preset.theme->contrast);
  palette.surface = adjustColor(preset.theme->surface, preset.theme->brightness, preset.theme->contrast);
  palette.primary = adjustColor(preset.theme->primary, preset.theme->brightness, preset.theme->contrast);
  palette.secondary = adjustColor(preset.theme->secondary, preset.theme->brightness, preset.theme->contrast);
  palette.accent = adjustColor(preset.theme->accent, preset.theme->brightness, preset.theme->contrast);
  palette.muted = adjustColor(preset.theme->muted, preset.theme->brightness, preset.theme->contrast);
  palette.border = palette.muted;
  palette.brightness = preset.theme->brightness;
  palette.contrast = preset.theme->contrast;
  return palette;
}
