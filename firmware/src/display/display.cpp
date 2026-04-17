#include "display.h"

#include "../core/alarm.h"

#include <string.h>

DisplayManager::DisplayManager()
    : preset_(ui::resolve(ui::defaultSelection())) {
  palette_ = buildThemePalette(preset_);
}

void DisplayManager::begin() {
  tft_.init();
  tft_.setRotation(1);
  tft_.fillScreen(TFT_BLACK);
  tft_.setTextDatum(MC_DATUM);
  geometry_ = buildLayoutGeometry(preset_, tft_.width(), tft_.height());
  ready_ = true;
}

void DisplayManager::setPreset(const ui::ResolvedPreset& preset) {
  preset_ = preset;
  palette_ = buildThemePalette(preset_);

  if (ready_) {
    geometry_ = buildLayoutGeometry(preset_, tft_.width(), tft_.height());
  }

  hasLastModel_ = false;
}

void DisplayManager::renderFull(const ClockSnapshot& snapshot) {
  if (!ready_) {
    return;
  }

  drawStaticLayer(tft_, preset_, palette_, geometry_);
  updateClock(snapshot, true);
}

void DisplayManager::renderMenuScreen(const char* title,
                                     const char* line1,
                                     const char* line2,
                                     const char* hint) {
  if (!ready_) {
    return;
  }

  tft_.fillScreen(palette_.background);
  tft_.setTextDatum(MC_DATUM);
  tft_.setTextColor(palette_.primary, palette_.background);
  tft_.setTextFont(4);
  tft_.drawString(title, tft_.width() / 2, 32);

  tft_.setTextFont(2);
  tft_.drawString(line1, tft_.width() / 2, 90);
  tft_.drawString(line2, tft_.width() / 2, 130);

  tft_.setTextDatum(BC_DATUM);
  tft_.setTextFont(1);
  tft_.setTextColor(palette_.secondary, palette_.background);
  tft_.drawString(hint, tft_.width() / 2, tft_.height() - 12);
}

void DisplayManager::updateClock(const ClockSnapshot& snapshot, bool force,
                                 const alarm_system::Schedule* alarm,
                                 bool alarmRinging) {
  if (!ready_) {
    return;
  }

  const ClockRenderModel model = buildClockRenderModel(preset_, snapshot, alarm, alarmRinging);
  if (!force && hasLastModel_ && modelsEqual(lastModel_, model)) {
    return;
  }

  drawClockLayer(tft_, preset_, palette_, geometry_, model,
                 hasLastModel_ ? &lastModel_ : nullptr);
  lastModel_ = model;
  hasLastModel_ = true;
}

bool DisplayManager::modelsEqual(const ClockRenderModel& left, const ClockRenderModel& right) const {
  return strcmp(left.primary, right.primary) == 0 &&
         strcmp(left.secondary, right.secondary) == 0 &&
         strcmp(left.date, right.date) == 0 &&
         left.showSecondary == right.showSecondary &&
         left.showDate == right.showDate &&
         left.colonVisible == right.colonVisible &&
         left.primaryFont == right.primaryFont &&
         left.secondaryFont == right.secondaryFont;
}
