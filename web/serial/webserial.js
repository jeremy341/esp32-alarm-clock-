export class WebSerialClient {
  constructor() {
    this.port = null;
    this.writer = null;
    this.reader = null;
    this.keepReading = false;
    this.onLog = null;
    this.decoder = new TextDecoder();
  }

  get connected() {
    return this.writer !== null;
  }

  async connect(onLog) {
    if (!("serial" in navigator)) {
      throw new Error("Web Serial is not available in this browser.");
    }

    this.onLog = onLog;

    if (this.connected) {
      await this.disconnect();
    }

    this.port = await navigator.serial.requestPort();
    await this.port.open({ baudRate: 115200 });
    this.writer = this.port.writable.getWriter();
    this.keepReading = true;

    if (this.port.readable) {
      this.reader = this.port.readable.getReader();
      this.readLoop();
    }
  }

  async readLoop() {
    try {
      while (this.keepReading && this.reader) {
        const { value, done } = await this.reader.read();
        if (done) {
          break;
        }

        if (value && this.onLog) {
          this.onLog(this.decoder.decode(value));
        }
      }
    } catch (error) {
      if (this.onLog) {
        this.onLog(`Serial read stopped: ${error.message}`);
      }
    }
  }

  async send(bytes) {
    if (!this.writer) {
      throw new Error("Connect to the ESP32 first.");
    }

    await this.writer.write(bytes);
  }

  async disconnect() {
    this.keepReading = false;

    if (this.reader) {
      await this.reader.cancel();
      this.reader.releaseLock();
      this.reader = null;
    }

    if (this.writer) {
      this.writer.releaseLock();
      this.writer = null;
    }

    if (this.port) {
      await this.port.close();
      this.port = null;
    }
  }
}
