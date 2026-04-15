#include "../display/display.h"

#include <string.h>

LayoutGeometry buildLayoutGeometry(const ui::ResolvedPreset& preset, int16_t screenWidth, int16_t screenHeight) {
  LayoutGeometry geometry{};

  if (strcmp(preset.layout->id, "minimal") == 0) {
    geometry.clock = {24, 54, static_cast<int16_t>(screenWidth - 48), 90};
    geometry.secondary = {40, 152, static_cast<int16_t>(screenWidth - 80), 20};
    geometry.date = {36, 182, static_cast<int16_t>(screenWidth - 72), 32};
    return geometry;
  }

  if (strcmp(preset.layout->id, "retro_pixel") == 0) {
    geometry.showHeader = true;
    geometry.showBorder = true;
    geometry.header = {26, 22, static_cast<int16_t>(screenWidth - 52), 28};
    geometry.clock = {28, 70, static_cast<int16_t>(screenWidth - 56), 74};
    geometry.secondary = {28, 154, static_cast<int16_t>(screenWidth - 56), 20};
    geometry.date = {28, 184, static_cast<int16_t>(screenWidth - 56), 28};
    return geometry;
  }

  geometry.showHeader = true;
  geometry.hasSecondaryPanel = true;
  geometry.header = {20, 18, static_cast<int16_t>(screenWidth - 40), 28};
  geometry.clock = {20, 64, 190, 98};
  geometry.secondary = {220, 64, 80, 98};
  geometry.date = {20, 180, static_cast<int16_t>(screenWidth - 40), 32};
  return geometry;
}
