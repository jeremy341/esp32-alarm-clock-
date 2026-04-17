#include "display.h"

#include <string.h>

namespace {

void clearPanel(TFT_eSPI& tft, const DisplayRect& rect, uint16_t fill, uint16_t border) {
  if (rect.width <= 0 || rect.height <= 0) {
    return;
  }

  tft.fillRoundRect(rect.x, rect.y, rect.width, rect.height, 10, fill);
  tft.drawRoundRect(rect.x, rect.y, rect.width, rect.height, 10, border);
}

}  // namespace

void drawClockLayer(TFT_eSPI& tft,
                    const ui::ResolvedPreset& preset,
                    const ThemePalette& palette,
                    const LayoutGeometry& geometry,
                    const ClockRenderModel& model,
                    const ClockRenderModel* previousModel) {
  const bool primaryChanged = previousModel == nullptr ||
                              strcmp(previousModel->primary, model.primary) != 0 ||
                              previousModel->primaryFont != model.primaryFont;
  const bool secondaryChanged = previousModel == nullptr ||
                                previousModel->showSecondary != model.showSecondary ||
                                strcmp(previousModel->secondary, model.secondary) != 0 ||
                                previousModel->secondaryFont != model.secondaryFont;
  const bool dateChanged = previousModel == nullptr ||
                           previousModel->showDate != model.showDate ||
                           strcmp(previousModel->date, model.date) != 0 ||
                           previousModel->showAlarmStatus != model.showAlarmStatus ||
                           previousModel->alarmRinging != model.alarmRinging ||
                           strcmp(previousModel->alarmStatus, model.alarmStatus) != 0;

  if (primaryChanged) {
    clearPanel(tft, geometry.clock, palette.surface, palette.border);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(palette.primary, palette.surface);
    tft.setTextFont(model.primaryFont);
    tft.drawString(model.primary,
                   geometry.clock.x + (geometry.clock.width / 2),
                   geometry.clock.y + (geometry.clock.height / 2));
  }

  if (geometry.hasSecondaryPanel) {
    if (secondaryChanged) {
      clearPanel(tft, geometry.secondary, palette.surface, palette.border);
    }
  } else if (geometry.secondary.height > 0 && secondaryChanged) {
    tft.fillRect(geometry.secondary.x, geometry.secondary.y, geometry.secondary.width,
                 geometry.secondary.height, palette.background);
  }

  if (secondaryChanged && model.showSecondary && geometry.secondary.height > 0) {
    const uint16_t secondaryBackground = geometry.hasSecondaryPanel ? palette.surface : palette.background;
    tft.setTextColor(palette.accent, secondaryBackground);
    tft.setTextFont(model.secondaryFont);
    tft.drawString(model.secondary,
                   geometry.secondary.x + (geometry.secondary.width / 2),
                   geometry.secondary.y + (geometry.secondary.height / 2));
  }

  if (!geometry.hasSecondaryPanel && geometry.secondary.height > 0) {
    tft.drawFastHLine(geometry.secondary.x, geometry.secondary.y + 1, geometry.secondary.width, palette.muted);
  }

  if (dateChanged && geometry.date.height > 0) {
    clearPanel(tft, geometry.date, palette.surface, palette.border);

    if (model.showAlarmStatus) {
      tft.setTextDatum(TL_DATUM);
      tft.setTextFont(1);
      tft.setTextColor(model.alarmRinging ? palette.accent : palette.secondary, palette.surface);
      tft.drawString(model.alarmStatus,
                     geometry.date.x + 8,
                     geometry.date.y + 8);
    }

    if (model.showDate) {
      tft.setTextDatum(MC_DATUM);
      tft.setTextFont(2);
      tft.setTextColor(palette.secondary, palette.surface);
      const int16_t dateY = geometry.date.y + (model.showAlarmStatus ? 20 : (geometry.date.height / 2));
      tft.drawString(model.date,
                     geometry.date.x + (geometry.date.width / 2),
                     dateY);
    }
  }

  if (strcmp(preset.layout->id, "retro_pixel") == 0) {
    tft.drawFastHLine(geometry.clock.x + 16, geometry.clock.y + geometry.clock.height - 16,
                      geometry.clock.width - 32, palette.muted);
  }
}
