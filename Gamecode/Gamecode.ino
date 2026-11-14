// openLUXbox

#include <FastLED.h>

// ---------------------------------------------------------------------------
// Konstanten
// ---------------------------------------------------------------------------

#define OLB_LED_PIN         2
#define OLB_NUM_LEDS        256
#define OLB_MATRIX_W        16
#define OLB_MATRIX_H        16
#define OLB_LED_TYPE        WS2812B
#define OLB_COLOR_ORDER     GRB
#define OLB_GAME_BRIGHTNESS 50
#define OLB_DEBOUNCE_MS     30

#define OLB_INIT_BRIGHTNESS      75
#define OLB_INIT_DIM_BRIGHTNESS  30
#define OLB_INIT_SCANLINE_MS     60      // ms pro Zeile beim Einblenden
#define OLB_INIT_PAUSE_MS        400     // Pause nachdem Wort vollständig
#define OLB_INIT_FLICKER_MS      80      // ms pro Flacker-Frame
#define OLB_INIT_FLICKER_COUNT   3       // Anzahl Flackerdurchläufe am Ende von "box"
#define OLB_INIT_FADEOUT_MS      60      // ms pro Zeile beim Ausblenden
#define OLB_INIT_NO_GAMES        7

CRGB OLB_LEDS[OLB_NUM_LEDS];

// Pinbelegung der Buttons
#if defined(ESP8266)
    #define OLB_BTN_LEFT     D5 // gelb
    #define OLB_BTN_RIGHT    D2 // grün
    #define OLB_BTN_UP       D6 // blau
    #define OLB_BTN_DOWN     D1 // weiß
    #define OLB_INIT_SEEDPIN D3
#elif defined(ESP32)
    #define OLB_BTN_LEFT     23 // gelb
    #define OLB_BTN_RIGHT    19 // grün
    #define OLB_BTN_UP       18 // blau
    #define OLB_BTN_DOWN     26 // weiß
    #define OLB_INIT_SEEDPIN 33
#endif

// ---------------------------------------------------------------------------
// Bilddaten: open (magenta), LUX (gelb), box (magenta)
// Jedes Bild: 16x16, zeilenweise, Index 0 = oben links
// ---------------------------------------------------------------------------

static const uint32_t OLB_FRAME_OPEN[256] PROGMEM = {
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0xff00ff,0x000000,0x000000,0xff00ff,0xff00ff,0x000000,
  0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,
  0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,0xff00ff,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0xff00ff,
  0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0x000000,0x000000,0x000000,0xff00ff,0x000000,0xff00ff,
  0x000000,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0x000000,0x000000,0x000000,0xff00ff,0xff00ff,0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,
  0x000000,0x000000,0x000000,0x000000,0xff00ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0xff00ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000
};

static const uint32_t OLB_FRAME_LUX[256] PROGMEM = {
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0xffff00,0x000000,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,
  0xffff00,0x000000,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,
  0xffff00,0x000000,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0xffff00,0x000000,0xffff00,0x000000,
  0xffff00,0x000000,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0xffff00,0x000000,0xffff00,0x000000,
  0xffff00,0x000000,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,
  0xffff00,0x000000,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0xffff00,0x000000,0xffff00,0x000000,
  0xffff00,0x000000,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0xffff00,0x000000,0xffff00,0x000000,
  0xffff00,0x000000,0x000000,0x000000,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,
  0xffff00,0xffff00,0xffff00,0xffff00,0x000000,0x000000,0xffff00,0xffff00,0xffff00,0x000000,0x000000,0xffff00,0x000000,0x000000,0x000000,0xffff00,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000
};

static const uint32_t OLB_FRAME_BOX[256] PROGMEM = {
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0xff00ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0xff00ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0xff00ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0xff00ff,0xff00ff,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0x000000,
  0x000000,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0x000000,
  0x000000,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0x000000,0x000000,0x000000,
  0x000000,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0xff00ff,0x000000,0x000000,
  0x000000,0xff00ff,0xff00ff,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0x000000,0x000000,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
  0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000
};

// ---------------------------------------------------------------------------
// Interne Hilfsfunktionen
// ---------------------------------------------------------------------------

// Wandelt Serpentinen-Koordinate (x,y) in LED-Index um
static inline uint16_t olb_xy(uint8_t x, uint8_t y) {
  if (y % 2 == 0) {
    return y * OLB_MATRIX_W + (OLB_MATRIX_W - 1 - x); // gerade: rechts→links
  } else {
    return y * OLB_MATRIX_W + x;                             // ungerade: links→rechts
  }
}


// Setzt eine komplette Zeile aus einem Frame mit gegebenem Helligkeitsfaktor (0..255)
// bright: 255 = volle Farbe, 128 = halbe Farbe
static void olb_setRow(CRGB* OLB_LEDS, const uint32_t* frame, uint8_t row, uint8_t bright) {
  for (uint8_t x = 0; x < OLB_MATRIX_W; x++) {
    uint32_t color = pgm_read_dword(&frame[row * OLB_MATRIX_W + x]);
    uint8_t r = ((color >> 16) & 0xFF);
    uint8_t g = ((color >>  8) & 0xFF);
    uint8_t b = ((color      ) & 0xFF);
    // Helligkeit skalieren
    r = (uint8_t)((uint16_t)r * bright / 255);
    g = (uint8_t)((uint16_t)g * bright / 255);
    b = (uint8_t)((uint16_t)b * bright / 255);
    OLB_LEDS[olb_xy(x, row)] = CRGB(r, g, b);
  }
}

// Löscht eine komplette Zeile
static void olb_clearRow(CRGB* OLB_LEDS, uint8_t row) {
  for (uint8_t x = 0; x < OLB_MATRIX_W; x++) {
    OLB_LEDS[olb_xy(x, row)] = CRGB::Black;
  }
}

// Löscht das gesamte Display
static void olb_clearAll(CRGB* OLB_LEDS) {
  for (uint16_t i = 0; i < OLB_NUM_LEDS; i++) {
    OLB_LEDS[i] = CRGB::Black;
  }
}

// Zeigt einen Frame (ein Wort) mit Scanline-Drop-Animation:
// 1. Zeilenweise einblenden: erst gedimmt, dann voll hell
// 2. Pause
// 3. Zeilenweise ausblenden (von oben nach unten)
static void olb_scanlineWord(CRGB* OLB_LEDS, const uint32_t* frame) {

  // --- Phase 1: Zeilenweise einblenden, erst gedimmt ---
  for (uint8_t row = 0; row < OLB_MATRIX_H; row++) {
    olb_setRow(OLB_LEDS, frame, row, OLB_INIT_DIM_BRIGHTNESS);
    FastLED.show();
    delay(OLB_INIT_SCANLINE_MS);
  }

  // --- Phase 1b: Alle Zeilen auf volle Helligkeit aufhellen ---
  for (uint8_t row = 0; row < OLB_MATRIX_H; row++) {
    olb_setRow(OLB_LEDS, frame, row, 255);
  }
  FastLED.show();

  // --- Phase 2: Pause ---
  delay(OLB_INIT_PAUSE_MS);

  // --- Phase 3: Zeilenweise ausblenden ---
  for (uint8_t row = 0; row < OLB_MATRIX_H; row++) {
    olb_clearRow(OLB_LEDS, row);
    FastLED.show();
    delay(OLB_INIT_FADEOUT_MS);
  }
}

// Zeigt das letzte Wort ("box") mit Scanline-Drop + Flackern am Ende statt Ausblenden
static void olb_scanlineWordFinal(CRGB* OLB_LEDS, const uint32_t* frame) {

  // --- Phase 1: Zeilenweise einblenden, erst gedimmt ---
  for (uint8_t row = 0; row < OLB_MATRIX_H; row++) {
    olb_setRow(OLB_LEDS, frame, row, OLB_INIT_DIM_BRIGHTNESS);
    FastLED.show();
    delay(OLB_INIT_SCANLINE_MS);
  }

  // --- Phase 1b: Alle Zeilen auf volle Helligkeit ---
  for (uint8_t row = 0; row < OLB_MATRIX_H; row++) {
    olb_setRow(OLB_LEDS, frame, row, 255);
  }
  FastLED.show();

  // --- Phase 2: Pause ---
  delay(OLB_INIT_PAUSE_MS);

  // --- Phase 3: Flackern (An/Aus) ---
  for (uint8_t i = 0; i < OLB_INIT_FLICKER_COUNT; i++) {
    // Aus
    olb_clearAll(OLB_LEDS);
    FastLED.show();
    delay(OLB_INIT_FLICKER_MS);
    // An
    for (uint8_t row = 0; row < OLB_MATRIX_H; row++) {
      olb_setRow(OLB_LEDS, frame, row, 255);
    }
    FastLED.show();
    delay(OLB_INIT_FLICKER_MS);
  }

  // --- Phase 4: Finales Ausblenden ---
  delay(OLB_INIT_PAUSE_MS);
  olb_clearAll(OLB_LEDS);
  FastLED.show();
}

// ── Echter Zufallsgenerator ───────────────────────────────
uint32_t getTrueRandom() {
  uint32_t seed = 0;
  for (int i = 0; i < 32; i++) {
    seed ^= (uint32_t)analogRead(A0) << (i % 16);
    delay(1);
  }
  for (int i = 0; i < 16; i++) {
    seed ^= (uint32_t)digitalRead(OLB_INIT_SEEDPIN) << i;
  }
  return seed;
}

unsigned int gameMenu() {
    int lastBtnLeft = digitalRead(OLB_BTN_LEFT);
    int lastBtnRight = digitalRead(OLB_BTN_RIGHT);
    int lastBtnDown = digitalRead(OLB_BTN_DOWN);

    unsigned long lastDebounceTimeLeft = 0;
    unsigned long lastDebounceTimeRight = 0;
    unsigned long lastDebounceTimeDown = 0;

    int game = 0;


    while (true) {
        unsigned long currentMillis = millis();
        int btnDown = digitalRead(OLB_BTN_DOWN);
        int btnLeft = digitalRead(OLB_BTN_LEFT);
        int btnRight = digitalRead(OLB_BTN_RIGHT);
        int select = 0;

        if (btnDown == LOW && lastBtnDown == HIGH && (currentMillis - lastDebounceTimeDown > OLB_DEBOUNCE_MS)) {
            lastDebounceTimeDown = currentMillis;
            return game;
        }

        if (btnLeft == LOW && lastBtnLeft == HIGH && (currentMillis - lastDebounceTimeLeft > OLB_DEBOUNCE_MS)) {
            lastDebounceTimeLeft = currentMillis;
            select = -1;
        }

        if (btnRight == LOW && lastBtnRight == HIGH && (currentMillis - lastDebounceTimeRight > OLB_DEBOUNCE_MS)) {
            lastDebounceTimeRight = currentMillis;
            select = 1;
        }

        game += select;
        if (game >= OLB_INIT_NO_GAMES) {
            game = 0;
        }
        if (game < 0) {
            game = OLB_INIT_NO_GAMES-1;
        }

        FastLED.clear();
        for (int i = 0; i < game+1; i++) {
            OLB_LEDS[i] = CRGB(255, 255, 255);
        }
        FastLED.show();

        lastBtnDown = btnDown;
        lastBtnLeft = btnLeft;
        lastBtnRight = btnRight;

        delay(20);
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println("openLUXbox startet...");

  pinMode(OLB_BTN_LEFT,  INPUT_PULLUP);
  pinMode(OLB_BTN_RIGHT, INPUT_PULLUP);
  pinMode(OLB_BTN_DOWN,  INPUT_PULLUP);
  pinMode(OLB_BTN_UP,    INPUT_PULLUP);

  FastLED.addLeds<WS2812B, OLB_LED_PIN, GRB>(OLB_LEDS, OLB_NUM_LEDS);
}

#include "4Pong/4Pong.h"
#include "Frog/frog.h"
#include "Kreuzfeuer/kreuzfeuer.h"
#include "Lux/lux.h"
#include "Nano_Hexagon/nano_hexagon.h"
#include "Shining/shining.h"
#include "Snake/snake.h"

void loop() {
    FastLED.setBrightness(OLB_INIT_BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
    randomSeed(getTrueRandom());

    olb_scanlineWord(OLB_LEDS,      OLB_FRAME_OPEN);
    olb_scanlineWord(OLB_LEDS,      OLB_FRAME_LUX);
    olb_scanlineWordFinal(OLB_LEDS, OLB_FRAME_BOX);

    unsigned int game = gameMenu();
    delay(500);

    FastLED.setBrightness(OLB_GAME_BRIGHTNESS);
    FastLED.clear();
    FastLED.show();

    switch (game) {
        case 0:
            FourPong::run();
            break;
        case 1:
            Frog::run();
            break;
        case 2:
            Kreuzfeuer::run();
            break;
        case 3:
            Lux::run();
            break;
        case 4:
            Nanohexagon::run();
            break;
        case 5:
            Shining::run();
            break;
        case 6:
            Snake::run();
            break;
    }
}
