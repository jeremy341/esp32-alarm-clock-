export function mountToolbar({ store, elements, actions }) {
  const { connectButton, applyButton, statusLabel, deviceLog } = elements;

  connectButton.addEventListener("click", actions.onConnect);
  applyButton.addEventListener("click", actions.onApply);

  store.subscribe((state) => {
    connectButton.textContent = state.connected ? "Reconnect ESP32" : "Connect ESP32";
    applyButton.disabled = !state.catalog;
    statusLabel.textContent = state.status;
    deviceLog.textContent = state.deviceLog || "No device output yet.";
  });
}
