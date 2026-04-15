function renderPreviewMarkup(model) {
  const layoutClass = model.layout.id === "digital" ? "digital" : "";
  const retroClass = model.layout.id === "retro_pixel" ? "retro" : "";
  const inlineSecondary = !model.layout.hasSecondsPanel && model.showSecondary
    ? `<div class="preview-secondary ${retroClass}">${model.secondary}</div>`
    : "";

  const secondsPanel = model.layout.hasSecondsPanel
    ? `<div class="preview-card"><div class="preview-secondary">${model.secondary}</div></div>`
    : "";

  const header = model.layout.showHeader
    ? `<div class="preview-header">${model.layout.label}</div>`
    : "";

  const date = model.showDate
    ? `<div class="preview-card"><div class="preview-date">${model.dateText}</div></div>`
    : "";

  return `
    <div
      class="preview-screen ${retroClass}"
      style="
        --preview-bg: ${model.theme.colors.background};
        --preview-surface: ${model.theme.colors.surface};
        --preview-primary: ${model.theme.colors.primary};
        --preview-secondary: ${model.theme.colors.secondary};
        --preview-accent: ${model.theme.colors.accent};
        --preview-border: ${model.theme.colors.muted};
      "
    >
      ${header}
      <div class="preview-main ${layoutClass}">
        <div class="preview-card">
          <div class="preview-clock ${retroClass}">${model.primary}</div>
        </div>
        ${secondsPanel}
      </div>
      ${inlineSecondary}
      ${date}
    </div>
  `;
}

export function mountPreviewRenderer({ store, elements }) {
  const {
    previewRoot,
    layoutTitle,
    layoutDescription,
    themeTitle,
    themeDescription,
    clockStyleTitle,
    clockStyleDescription,
  } = elements;

  store.subscribe((state) => {
    if (!state.previewModel) {
      return;
    }

    previewRoot.innerHTML = renderPreviewMarkup(state.previewModel);
    layoutTitle.textContent = state.previewModel.layout.label;
    layoutDescription.textContent = state.previewModel.layout.description;
    themeTitle.textContent = state.previewModel.theme.label;
    themeDescription.textContent = `${state.previewModel.theme.description} Brightness ${state.previewModel.theme.brightness}, contrast ${state.previewModel.theme.contrast}.`;
    clockStyleTitle.textContent = state.previewModel.clockStyle.label;
    clockStyleDescription.textContent = state.previewModel.clockStyle.description;
  });
}
