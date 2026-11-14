#!/bin/sh
arduino-cli compile -v -e -b esp8266:esp8266:nodemcu Gamecode
cp Gamecode/build/esp8266.esp8266.nodemcu/*.bin firmware/ESP8266/
arduino-cli compile -v -e -b esp32:esp32:mhetesp32minikit Gamecode
cp Gamecode/build/esp32.esp32.mhetesp32minikit/*.bin firmware/ESP32/
cp Gamecode/build/esp32.esp32.mhetesp32minikit/partitions.csv firmware/ESP32/
cp Gamecode/build/esp32.esp32.mhetesp32minikit/flash_args firmware/ESP32/
