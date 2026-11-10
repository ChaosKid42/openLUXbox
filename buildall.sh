#!/bin/sh
arduino-cli compile -v -e -b esp8266:esp8266:nodemcu Gamecode
mv Gamecode/build/esp8266.esp8266.nodemcu/Gamecode.ino.bin Gamecode_ESP8266.bin
arduino-cli compile -v -e -b esp32:esp32:mhetesp32minikit Gamecode
mv Gamecode/build/esp32.esp32.mhetesp32minikit/Gamecode.ino.bootloader.bin .
mv Gamecode/build/esp32.esp32.mhetesp32minikit/Gamecode.ino.partitions.bin .
mv Gamecode/build/esp32.esp32.mhetesp32minikit/boot_app0.bin .
mv Gamecode/build/esp32.esp32.mhetesp32minikit/Gamecode.ino.bin Gamecode_ESP32.bin
rm -r Gamecode/build
