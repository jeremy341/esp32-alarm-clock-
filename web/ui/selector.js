function syncSelectOptions(select, items, value) {
  const optionsHtml = items
    .map((item) => `<option value="${item.id}">${item.label}</option>`)
    .join("");

  if (select.dataset.loaded !== "true") {
    select.innerHTML = optionsHtml;
    select.dataset.loaded = "true";
  }

  select.value = value;
}

export function mountSelectors({ store, elements }) {
  const { layoutSelect, themeSelect, clockStyleSelect } = elements;

  layoutSelect.addEventListener("change", (event) => {
    store.update((state) => ({
      ...state,
      selection: { ...state.selection, layout: event.target.value },
    }));
  });

  themeSelect.addEventListener("change", (event) => {
    store.update((state) => ({
      ...state,
      selection: { ...state.selection, theme: event.target.value },
    }));
  });

  clockStyleSelect.addEventListener("change", (event) => {
    store.update((state) => ({
      ...state,
      selection: { ...state.selection, clockStyle: event.target.value },
    }));
  });

  store.subscribe((state) => {
    if (!state.catalog) {
      return;
    }

    syncSelectOptions(layoutSelect, state.catalog.layouts, state.selection.layout);
    syncSelectOptions(themeSelect, state.catalog.themes, state.selection.theme);
    syncSelectOptions(clockStyleSelect, state.catalog.clockStyles, state.selection.clockStyle);
  });
}
