# ESP32 Alarm Clock

Preset-driven ESP32 alarm clock built for:

- ESP32-WROOM with CH340 USB serial bridge
- ILI9341 SPI TFT using `TFT_eSPI`
- DS3231 RTC on I2C
- LittleFS persistent storage
- Web Serial configuration from a plain browser app

## Core idea

This project is not bitmap-based.

The browser only sends JSON preset selections:

- layout
- theme
- clock style

The ESP32 renders the full UI dynamically on-device and updates only the clock region every second to reduce redraw flicker.

## Included presets

### Layouts

- `minimal`
- `digital`
- `retro_pixel`

### Themes

- `oled_black`
- `neon_blue`
- `matrix_green`

### Clock styles

- `clean_24h`
- `split_seconds`
- `retro_12h`

## Firmware

The firmware:

- reads time from the DS3231
- falls back to compile time if the RTC is missing
- loads the last preset selection from LittleFS at boot
- accepts JSON preset packets over USB serial
- validates packets with CRC16
- redraws static chrome only when the preset changes
- redraws only the dynamic clock area on live updates

## Web app

The web app:

- loads preset catalogs from `web/presets/*.json`
- shows a live preview of the selected combination
- connects to the ESP32 via Web Serial
- sends the selected preset as JSON with a binary packet envelope and CRC

## PlatformIO

`firmware/platformio.ini` is configured for:

- `esp32dev`
- `TFT_eSPI`
- `RTClib`
- `ArduinoJson`
- `LittleFS`

## Web usage

Serve the `web/` folder over `http://localhost`, for example:

```powershell
cd web
python -m http.server 8000
```

Then open:

```text
http://localhost:8000
```

Use a Chromium-based browser with Web Serial enabled.
