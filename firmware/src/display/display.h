#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "../time/clock.h"
#include "../ui/presets.h"

namespace alarm_system {
struct Schedule;
}

struct DisplayRect {
  int16_t x = 0;
  int16_t y = 0;
  int16_t width = 0;
  int16_t height = 0;
};

struct LayoutGeometry {
  DisplayRect header;
  DisplayRect clock;
  DisplayRect secondary;
  DisplayRect date;
  bool showHeader = false;
  bool hasSecondaryPanel = false;
  bool showBorder = false;
};

struct ThemePalette {
  uint16_t background = TFT_BLACK;
  uint16_t surface = TFT_BLACK;
  uint16_t primary = TFT_WHITE;
  uint16_t secondary = 0xC618;
  uint16_t accent = TFT_CYAN;
  uint16_t muted = TFT_DARKGREY;
  uint16_t border = TFT_DARKGREY;
  uint8_t brightness = 255;
  uint8_t contrast = 255;
};

struct ClockRenderModel {
  char primary[12] = "";
  char secondary[16] = "";
  char date[24] = "";
  bool showSecondary = false;
  bool showDate = false;
  bool colonVisible = true;
  bool showAlarmStatus = false;
  bool alarmRinging = false;
  char alarmStatus[32] = "";
  uint8_t primaryFont = 7;
  uint8_t secondaryFont = 2;
};

ThemePalette buildThemePalette(const ui::ResolvedPreset& preset);
LayoutGeometry buildLayoutGeometry(const ui::ResolvedPreset& preset, int16_t screenWidth, int16_t screenHeight);
ClockRenderModel buildClockRenderModel(const ui::ResolvedPreset& preset,
                                      const ClockSnapshot& snapshot,
                                      const alarm_system::Schedule* alarm = nullptr,
                                      bool alarmRinging = false);

void drawStaticLayer(TFT_eSPI& tft,
                     const ui::ResolvedPreset& preset,
                     const ThemePalette& palette,
                     const LayoutGeometry& geometry);

void drawClockLayer(TFT_eSPI& tft,
                    const ui::ResolvedPreset& preset,
                    const ThemePalette& palette,
                    const LayoutGeometry& geometry,
                    const ClockRenderModel& model,
                    const ClockRenderModel* previousModel = nullptr);

void renderMenuScreen(TFT_eSPI& tft,
                      const ThemePalette& palette,
                      const char* title,
                      const char* line1,
                      const char* line2,
                      const char* hint);

class DisplayManager {
public:
  DisplayManager();

  void begin();
  void setPreset(const ui::ResolvedPreset& preset);
  void renderFull(const ClockSnapshot& snapshot);
  void renderMenuScreen(const char* title,
                        const char* line1,
                        const char* line2,
                        const char* hint);
  void updateClock(const ClockSnapshot& snapshot, bool force = false,
                   const alarm_system::Schedule* alarm = nullptr,
                   bool alarmRinging = false);

private:
  bool modelsEqual(const ClockRenderModel& left, const ClockRenderModel& right) const;

  TFT_eSPI tft_;
  ui::ResolvedPreset preset_;
  ThemePalette palette_;
  LayoutGeometry geometry_;
  ClockRenderModel lastModel_;
  bool ready_ = false;
  bool hasLastModel_ = false;
};
