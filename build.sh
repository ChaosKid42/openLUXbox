#!/bin/sh
mkdir -p firmware/binaries/ESP8266
mkdir -p firmware/binaries/ESP32
arduino-cli compile -v -e -b esp8266:esp8266:nodemcu firmware/src/openLUXbox
cp firmware/src/openLUXbox/build/esp8266.esp8266.nodemcu/*.bin firmware/binaries/ESP8266/
arduino-cli compile -v -e -b esp32:esp32:mhetesp32minikit firmware/src/openLUXbox
cp firmware/src/openLUXbox/build/esp32.esp32.mhetesp32minikit/*.bin firmware/binaries/ESP32/
cp firmware/src/openLUXbox/build/esp32.esp32.mhetesp32minikit/partitions.csv firmware/binaries/ESP32/
cp firmware/src/openLUXbox/build/esp32.esp32.mhetesp32minikit/flash_args firmware/binaries/ESP32/
