export function createStore(initialState) {
  let state = initialState;
  const listeners = new Set();

  function notify() {
    listeners.forEach((listener) => listener(state));
  }

  return {
    get() {
      return state;
    },

    set(patch) {
      state = { ...state, ...patch };
      notify();
    },

    update(updater) {
      state = updater(state);
      notify();
    },

    subscribe(listener) {
      listeners.add(listener);
      listener(state);
      return () => listeners.delete(listener);
    },
  };
}
