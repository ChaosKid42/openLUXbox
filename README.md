# openLUXbox
## DIY-Open-Source-Coop-**Spielekonsole**

*openLUXbox ist eine offene DIY-Spielekonsole für unter 40€ – gebaut zum Spielen, gedacht zum Weitermachen.*

### Fork der Software als Monolith.

Die ist ein Fork der Originalsoftware von Jochen Wilke (https://github.com/Apologus/openLUXbox), der alle vorhandenen Spiele in ein einziges Arduino-Sketch
zusammenführt. Nach dem Start der openLUXbox erscheint ein Auswahlmenü, mit Hilfe dessen
man eines der acht Spiele auswählen kann. Auswahl mit links/rechts bzw. gelb/grün. Start mit unten bzw. weiß.

Eine Übersicht aller Spiele mit Beschreibungen und Screenshots findet sich in [GAMES.md](GAMES.md).

Die Software fuktioniet sowohl mit einem ESP8266 als auch mit einem ESP32.

### Verzeichnisstruktur

```
openLUXbox/
├── firmware/
│   ├── src/openLUXbox/      Arduino-Sketch (Hauptprogramm + Spiele)
│   │   ├── openLUXbox.ino   Start, Auswahlmenü, Spielaufrufe
│   │   ├── 4Pong/           Spiele als Header-Dateien
│   │   ├── CanoLbalt/
│   │   ├── Frog/
│   │   ├── Kreuzfeuer/
│   │   ├── Lux/
│   │   ├── Nano_Hexagon/
│   │   ├── Shining/
│   │   └── Snake/
│   └── binaries/            Fertig kompilierte Firmware
│       ├── ESP8266/         openLUXbox.ino.bin + openLUXbox.json (Webflasher-Manifest)
│       └── ESP32/           Bootloader, Partitionen, boot_app0, openLUXbox.ino.bin + openLUXbox.json
├── pictures/                Screenshots und Logo für die Dokumentation
├── build.sh                 Baut die Firmware für ESP8266 und ESP32
├── GAMES.md                 Spieleübersicht
└── README.md
```

### Bauen

Voraussetzung ist die [arduino-cli](https://arduino.github.io/arduino-cli/) mit den Cores
`esp8266:esp8266` und `esp32:esp32` sowie der Bibliothek `FastLED`.

```sh
./build.sh
```

Das Skript kompiliert `firmware/src/openLUXbox` für beide Boards und kopiert die Ergebnisse
nach `firmware/binaries/ESP8266/` bzw. `firmware/binaries/ESP32/`. Das temporäre
Build-Verzeichnis `firmware/src/openLUXbox/build` wird nicht versioniert.

### Flashen

Die fertigen Binaries liegen in `firmware/binaries/`. Geflasht wird z.B.
mit `arduino-cli`/`esptool` aus dem Sketch-Verzeichnis `firmware/src/openLUXbox`
oder direkt aus dem Binaries-Verzeichnis `firmware/binaries` mit dem `esptool`.
