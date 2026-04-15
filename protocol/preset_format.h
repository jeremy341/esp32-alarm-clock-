#pragma once

#include <stddef.h>

namespace alarm_clock_format {

static constexpr size_t kIdBufferSize = 24;

static constexpr char kFieldLayout[] = "layout";
static constexpr char kFieldTheme[] = "theme";
static constexpr char kFieldClockStyle[] = "clockStyle";
static constexpr char kFieldVersion[] = "version";

}  // namespace alarm_clock_format
