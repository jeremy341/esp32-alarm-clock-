#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../../../protocol/preset_format.h"

namespace ui {

struct LayoutDefinition {
  const char* id;
  const char* label;
  const char* description;
  bool showHeader;
  bool hasSecondsPanel;
  bool showBorder;
};

struct ThemeDefinition {
  const char* id;
  const char* label;
  const char* description;
  uint16_t background;
  uint16_t surface;
  uint16_t primary;
  uint16_t secondary;
  uint16_t accent;
  uint16_t muted;
  uint8_t brightness;
  uint8_t contrast;
};

struct ClockStyleDefinition {
  const char* id;
  const char* label;
  const char* description;
  bool hour24;
  bool blinkColon;
  bool showDate;
  bool splitSeconds;
  bool uppercaseDate;
  uint8_t primaryFont;
  uint8_t secondaryFont;
};

struct PresetSelection {
  char layoutId[alarm_clock_format::kIdBufferSize];
  char themeId[alarm_clock_format::kIdBufferSize];
  char clockStyleId[alarm_clock_format::kIdBufferSize];
};

struct ResolvedPreset {
  const LayoutDefinition* layout;
  const ThemeDefinition* theme;
  const ClockStyleDefinition* clockStyle;
};

constexpr uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue) {
  return static_cast<uint16_t>(((red & 0xF8U) << 8U) |
                               ((green & 0xFCU) << 3U) |
                               (blue >> 3U));
}

static const LayoutDefinition kLayouts[] = {
    {"minimal", "Minimal", "Large centered time with a clean footer date.", false, false, false},
    {"digital", "Digital", "Dashboard layout with header and dedicated seconds panel.", true, true, false},
    {"retro_pixel", "Retro Pixel", "Arcade-inspired framing with bold contrast.", true, false, true},
};

static const ThemeDefinition kThemes[] = {
    {"oled_black", "OLED Black", "High contrast cyan on true black.",
     rgb565(0, 0, 0), rgb565(14, 18, 24), rgb565(109, 240, 255), rgb565(130, 154, 181),
     rgb565(255, 159, 67), rgb565(52, 61, 74), 196, 248},
    {"neon_blue", "Neon Blue", "Electric blue accents with deep navy panels.",
     rgb565(5, 10, 25), rgb565(12, 28, 56), rgb565(58, 193, 255), rgb565(175, 222, 255),
     rgb565(255, 84, 112), rgb565(44, 68, 92), 220, 236},
    {"matrix_green", "Matrix Green", "Green phosphor look with amber highlights.",
     rgb565(1, 7, 2), rgb565(9, 24, 10), rgb565(115, 255, 135), rgb565(180, 255, 190),
     rgb565(255, 182, 77), rgb565(32, 56, 35), 208, 228},
};

static const ClockStyleDefinition kClockStyles[] = {
    {"clean_24h", "Clean 24H", "Modern 24-hour display with blinking colon.", true, true, true, false, true, 7, 2},
    {"split_seconds", "Split Seconds", "24-hour display with dedicated seconds field.", true, true, true, true, true, 7, 4},
    {"retro_12h", "Retro 12H", "12-hour clock with AM/PM secondary text.", false, false, true, false, true, 6, 2},
};

inline void copyId(char* destination, size_t size, const char* source) {
  if (size == 0U) {
    return;
  }

  if (source == nullptr) {
    destination[0] = '\0';
    return;
  }

  snprintf(destination, size, "%s", source);
}

inline PresetSelection defaultSelection() {
  PresetSelection selection{};
  copyId(selection.layoutId, sizeof(selection.layoutId), kLayouts[1].id);
  copyId(selection.themeId, sizeof(selection.themeId), kThemes[0].id);
  copyId(selection.clockStyleId, sizeof(selection.clockStyleId), kClockStyles[0].id);
  return selection;
}

inline const LayoutDefinition* findLayout(const char* id) {
  for (size_t index = 0; index < (sizeof(kLayouts) / sizeof(kLayouts[0])); ++index) {
    if (strcmp(kLayouts[index].id, id) == 0) {
      return &kLayouts[index];
    }
  }

  return nullptr;
}

inline const ThemeDefinition* findTheme(const char* id) {
  for (size_t index = 0; index < (sizeof(kThemes) / sizeof(kThemes[0])); ++index) {
    if (strcmp(kThemes[index].id, id) == 0) {
      return &kThemes[index];
    }
  }

  return nullptr;
}

inline const ClockStyleDefinition* findClockStyle(const char* id) {
  for (size_t index = 0; index < (sizeof(kClockStyles) / sizeof(kClockStyles[0])); ++index) {
    if (strcmp(kClockStyles[index].id, id) == 0) {
      return &kClockStyles[index];
    }
  }

  return nullptr;
}

inline bool normalizeSelection(PresetSelection& selection) {
  const LayoutDefinition* layout = findLayout(selection.layoutId);
  const ThemeDefinition* theme = findTheme(selection.themeId);
  const ClockStyleDefinition* style = findClockStyle(selection.clockStyleId);

  if (layout == nullptr) {
    copyId(selection.layoutId, sizeof(selection.layoutId), kLayouts[1].id);
    layout = &kLayouts[1];
  }

  if (theme == nullptr) {
    copyId(selection.themeId, sizeof(selection.themeId), kThemes[0].id);
    theme = &kThemes[0];
  }

  if (style == nullptr) {
    copyId(selection.clockStyleId, sizeof(selection.clockStyleId), kClockStyles[0].id);
    style = &kClockStyles[0];
  }

  return (layout != nullptr) && (theme != nullptr) && (style != nullptr);
}

inline ResolvedPreset resolve(const PresetSelection& selection) {
  PresetSelection normalized = selection;
  normalizeSelection(normalized);

  ResolvedPreset preset{};
  preset.layout = findLayout(normalized.layoutId);
  preset.theme = findTheme(normalized.themeId);
  preset.clockStyle = findClockStyle(normalized.clockStyleId);
  return preset;
}

inline bool equals(const PresetSelection& left, const PresetSelection& right) {
  return strcmp(left.layoutId, right.layoutId) == 0 &&
         strcmp(left.themeId, right.themeId) == 0 &&
         strcmp(left.clockStyleId, right.clockStyleId) == 0;
}

}  // namespace ui
