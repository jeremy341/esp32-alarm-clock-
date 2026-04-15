import { loadPresetCatalog, defaultSelection, normalizeSelection, resolveSelection } from "./core/preset_engine.js";
import { buildPreviewModel } from "./core/preview.js";
import { createStore } from "./core/state.js";
import { mountPreviewRenderer } from "./ui/preview_renderer.js";
import { mountSelectors } from "./ui/selector.js";
import { mountToolbar } from "./ui/toolbar.js";
import { WebSerialClient } from "./serial/webserial.js";
import { uploadPreset } from "./serial/uploader.js";

const SELECTION_STORAGE_KEY = "esp32-alarm-clock-selection";

const store = createStore({
  catalog: null,
  selection: defaultSelection(),
  previewModel: null,
  connected: false,
  status: "Loading presets...",
  deviceLog: "",
});

const client = new WebSerialClient();

const elements = {
  layoutSelect: document.getElementById("layout-select"),
  themeSelect: document.getElementById("theme-select"),
  clockStyleSelect: document.getElementById("clock-style-select"),
  connectButton: document.getElementById("connect-button"),
  applyButton: document.getElementById("apply-button"),
  statusLabel: document.getElementById("status-label"),
  deviceLog: document.getElementById("device-log"),
  previewRoot: document.getElementById("preview-root"),
  layoutTitle: document.getElementById("layout-title"),
  layoutDescription: document.getElementById("layout-description"),
  themeTitle: document.getElementById("theme-title"),
  themeDescription: document.getElementById("theme-description"),
  clockStyleTitle: document.getElementById("clock-style-title"),
  clockStyleDescription: document.getElementById("clock-style-description"),
};

function restoreSelection() {
  try {
    const raw = localStorage.getItem(SELECTION_STORAGE_KEY);
    return raw ? JSON.parse(raw) : defaultSelection();
  } catch {
    return defaultSelection();
  }
}

function persistSelection(selection) {
  localStorage.setItem(SELECTION_STORAGE_KEY, JSON.stringify(selection));
}

mountSelectors({
  store,
  elements,
});

mountPreviewRenderer({
  store,
  elements,
});

mountToolbar({
  store,
  elements,
  actions: {
    onConnect: async () => {
      try {
        store.set({ status: "Opening Web Serial connection..." });
        await client.connect((logChunk) => {
          store.update((state) => ({
            ...state,
            deviceLog: `${state.deviceLog}${state.deviceLog ? "" : ""}${logChunk}`.slice(-3000),
          }));
        });
        store.set({
          connected: true,
          status: "Connected. The ESP32 is ready for preset upload.",
        });
      } catch (error) {
        store.set({
          connected: false,
          status: error.message,
        });
      }
    },

    onApply: async () => {
      try {
        const state = store.get();
        if (!state.catalog) {
          return;
        }

        const normalizedSelection = normalizeSelection(state.catalog, state.selection);
        store.set({ status: "Uploading preset JSON..." });
        await uploadPreset(client, normalizedSelection);
        store.set({
          selection: normalizedSelection,
          status: "Preset applied. The ESP32 will persist it to LittleFS.",
        });
      } catch (error) {
        store.set({ status: error.message });
      }
    },
  },
});

store.subscribe((state) => {
  if (state.catalog) {
    persistSelection(state.selection);
  }
});

store.subscribe((state) => {
  if (!state.catalog) {
    return;
  }

  const resolved = resolveSelection(state.catalog, state.selection);
  const nextPreview = buildPreviewModel(resolved, new Date());

  if (!state.previewModel ||
      state.previewModel.layout.id !== nextPreview.layout.id ||
      state.previewModel.theme.id !== nextPreview.theme.id ||
      state.previewModel.clockStyle.id !== nextPreview.clockStyle.id) {
    store.set({
      selection: resolved.selection,
      previewModel: nextPreview,
    });
  }
});

setInterval(() => {
  const state = store.get();
  if (!state.catalog) {
    return;
  }

  const resolved = resolveSelection(state.catalog, state.selection);
  store.set({
    previewModel: buildPreviewModel(resolved, new Date()),
  });
}, 1000);

async function boot() {
  try {
    const catalog = await loadPresetCatalog();
    const selection = normalizeSelection(catalog, restoreSelection());
    const resolved = resolveSelection(catalog, selection);

    store.set({
      catalog,
      selection,
      previewModel: buildPreviewModel(resolved, new Date()),
      status: "Choose a preset, connect the ESP32, then apply it.",
    });
  } catch (error) {
    store.set({ status: error.message });
  }
}

boot();
