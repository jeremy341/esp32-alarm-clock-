function pad2(value) {
  return String(value).padStart(2, "0");
}

function weekday(date) {
  return ["SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"][date.getDay()];
}

function month(date) {
  return ["JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"][date.getMonth()];
}

function buildPrimaryTime(layout, clockStyle, date) {
  let hour = date.getHours();

  if (!clockStyle.hour24) {
    hour = hour % 12 || 12;
  }

  const colonVisible = !clockStyle.blinkColon || date.getSeconds() % 2 === 0;
  const separator = colonVisible ? ":" : " ";
  return `${pad2(hour)}${separator}${pad2(date.getMinutes())}`;
}

function buildSecondaryText(layout, clockStyle, date) {
  if (!clockStyle.hour24) {
    return `${date.getHours() < 12 ? "AM" : "PM"} ${pad2(date.getSeconds())}`;
  }

  if (clockStyle.splitSeconds || layout.hasSecondsPanel) {
    return pad2(date.getSeconds());
  }

  return "";
}

export function buildPreviewModel(resolved, date = new Date()) {
  const { layout, theme, clockStyle } = resolved;
  const primary = buildPrimaryTime(layout, clockStyle, date);
  const secondary = buildSecondaryText(layout, clockStyle, date);

  return {
    primary,
    secondary,
    showSecondary: Boolean(secondary),
    showDate: clockStyle.showDate,
    dateText: `${weekday(date)} ${pad2(date.getDate())} ${month(date)} ${date.getFullYear()}`,
    layout,
    theme,
    clockStyle,
  };
}
