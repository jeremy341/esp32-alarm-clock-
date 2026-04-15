const PRESET_PATHS = {
  layouts: "./presets/layouts.json",
  themes: "./presets/themes.json",
  clockStyles: "./presets/clock_styles.json",
};

export async function loadPresetCatalog() {
  const [layouts, themes, clockStyles] = await Promise.all(
    Object.values(PRESET_PATHS).map(async (path) => {
      const response = await fetch(path);
      if (!response.ok) {
        throw new Error(`Failed to load ${path}`);
      }

      return response.json();
    }),
  );

  return { layouts, themes, clockStyles };
}

export function defaultSelection() {
  return {
    layout: "digital",
    theme: "oled_black",
    clockStyle: "clean_24h",
  };
}

function findById(items, id) {
  return items.find((item) => item.id === id) ?? items[0];
}

export function normalizeSelection(catalog, selection) {
  const defaults = defaultSelection();
  const layout = findById(catalog.layouts, selection?.layout ?? defaults.layout);
  const theme = findById(catalog.themes, selection?.theme ?? defaults.theme);
  const clockStyle = findById(catalog.clockStyles, selection?.clockStyle ?? defaults.clockStyle);

  return {
    layout: layout.id,
    theme: theme.id,
    clockStyle: clockStyle.id,
  };
}

export function resolveSelection(catalog, selection) {
  const normalized = normalizeSelection(catalog, selection);
  return {
    selection: normalized,
    layout: findById(catalog.layouts, normalized.layout),
    theme: findById(catalog.themes, normalized.theme),
    clockStyle: findById(catalog.clockStyles, normalized.clockStyle),
  };
}
