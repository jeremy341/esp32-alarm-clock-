#include "display.h"

namespace {

void fillPanel(TFT_eSPI& tft, const DisplayRect& rect, uint16_t fill, uint16_t border) {
  if (rect.width <= 0 || rect.height <= 0) {
    return;
  }

  tft.fillRoundRect(rect.x, rect.y, rect.width, rect.height, 10, fill);
  tft.drawRoundRect(rect.x, rect.y, rect.width, rect.height, 10, border);
}

}  // namespace

void drawStaticLayer(TFT_eSPI& tft,
                     const ui::ResolvedPreset& preset,
                     const ThemePalette& palette,
                     const LayoutGeometry& geometry) {
  tft.fillScreen(palette.background);

  if (geometry.showBorder) {
    tft.drawRect(10, 10, tft.width() - 20, tft.height() - 20, palette.accent);
    tft.drawRect(16, 16, tft.width() - 32, tft.height() - 32, palette.muted);
  }

  if (geometry.showHeader) {
    fillPanel(tft, geometry.header, palette.surface, palette.border);
    tft.setTextColor(palette.accent, palette.surface);
    tft.setTextFont(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(preset.layout->label, geometry.header.x + (geometry.header.width / 2),
                   geometry.header.y + (geometry.header.height / 2));
  }

  fillPanel(tft, geometry.clock, palette.surface, palette.border);

  if (geometry.hasSecondaryPanel) {
    fillPanel(tft, geometry.secondary, palette.surface, palette.border);
  }

  if (geometry.date.height > 0) {
    fillPanel(tft, geometry.date, palette.surface, palette.border);
  }

  if (!geometry.hasSecondaryPanel && geometry.secondary.height > 0) {
    tft.drawFastHLine(geometry.secondary.x, geometry.secondary.y + 1, geometry.secondary.width, palette.muted);
  }
}
