namespace Snake
{
    // ============================================================================
    // 2. DEFINES & KONSTANTEN
    // ============================================================================
    // Hardware Pins

    // Gameplay Parameter
    const unsigned int INITIAL_SNAKE_LENGTH = 3;
    const unsigned int MAX_SNAKE_LENGTH = 256;
    const unsigned int INITIAL_SPEED_MS = 220;
    const unsigned int MIN_SPEED_MS = 80;
    const unsigned int SPEED_DECREMENT_MS = 3;
    const unsigned int GAMEOVER_DELAY_MS = 1200;

    // ============================================================================
    // 3. ENUMS & STRUCTS
    // ============================================================================
    enum GameState {
      STATE_TITLE,
      STATE_PLAYING,
      STATE_GAMEOVER,
      STATE_SCORE
    };

    enum Direction {
      DIR_NONE,
      DIR_UP,
      DIR_DOWN,
      DIR_LEFT,
      DIR_RIGHT
    };

    struct Point {
      int8_t x;
      int8_t y;
    };

    struct Button {
      uint8_t pin;
      uint8_t lastState;
      uint32_t lastDebounceTime;
    };

    // ============================================================================
    // 4. FUNKTIONSPROTOTYPEN
    // ============================================================================
    void setPixel(int8_t x, int8_t y, CRGB color);
    void handleInput();
    void resetGame();
    void spawnFood();
    void updateGame();
    void drawTitle();
    void drawGame();
    void drawGameOver();
    void drawScore();
    void drawDigit(uint8_t startX, uint8_t startY, uint8_t digit, CRGB color);

    // ============================================================================
    // 5. GLOBALE VARIABLEN
    // ============================================================================

    // Spielzustand
    GameState currentState = STATE_TITLE;
    uint32_t stateTimer = 0;

    // Buttons
    Button btnUp    = { OLB_BTN_UP,    HIGH, 0 };
    Button btnDown  = { OLB_BTN_DOWN,  HIGH, 0 };
    Button btnLeft  = { OLB_BTN_LEFT,  HIGH, 0 };
    Button btnRight = { OLB_BTN_RIGHT, HIGH, 0 };

    bool btnUpPressed    = false;
    bool btnDownPressed  = false;
    bool btnLeftPressed  = false;
    bool btnRightPressed = false;

    // Schlange & Futter
    Point snake[MAX_SNAKE_LENGTH];
    uint16_t snakeLength = INITIAL_SNAKE_LENGTH;
    Direction currentDir = DIR_RIGHT;
    Direction nextDir    = DIR_RIGHT;
    Point food           = { 0, 0 };

    // Gameplay-Timer & Score
    uint32_t lastStepTime = 0;
    uint16_t currentSpeedMs = INITIAL_SPEED_MS;
    uint16_t score = 0;

    // 3x5 Bitmap-Schriftart für Ziffern 0–9
    const uint8_t FONT_3X5[10][5] = {
      { 0b111, 0b101, 0b101, 0b101, 0b111 }, // 0
      { 0b010, 0b110, 0b010, 0b010, 0b111 }, // 1
      { 0b111, 0b001, 0b111, 0b100, 0b111 }, // 2
      { 0b111, 0b001, 0b111, 0b001, 0b111 }, // 3
      { 0b101, 0b101, 0b111, 0b001, 0b001 }, // 4
      { 0b111, 0b100, 0b111, 0b001, 0b111 }, // 5
      { 0b111, 0b100, 0b111, 0b101, 0b111 }, // 6
      { 0b111, 0b001, 0b010, 0b010, 0b010 }, // 7
      { 0b111, 0b101, 0b111, 0b101, 0b111 }, // 8
      { 0b111, 0b101, 0b111, 0b001, 0b111 }  // 9
    };

    // ============================================================================
    // 6. RUN
    // ============================================================================
    void run() {
      Serial.println(F("\n[openLUXbox] 4-Player Co-op Snake starting..."));
      currentState = STATE_TITLE;
      while (true) {
          handleInput();

          switch (currentState) {
            case STATE_TITLE:
              if (btnUpPressed || btnDownPressed || btnLeftPressed || btnRightPressed) {
                resetGame();
                currentState = STATE_PLAYING;
              }
              drawTitle();
              break;

            case STATE_PLAYING:
              updateGame();
              drawGame();
              break;

            case STATE_GAMEOVER:
              drawGameOver();
              if (millis() - stateTimer >= GAMEOVER_DELAY_MS) {
                currentState = STATE_SCORE;
              }
              break;

            case STATE_SCORE:
              if (btnUpPressed || btnDownPressed || btnLeftPressed || btnRightPressed) {
                currentState = STATE_TITLE;
              }
              drawScore();
              break;
          }

          FastLED.show();
      }
    }

    // ============================================================================
    // 8. HILFS- & HARDWARE-FUNKTIONEN
    // ============================================================================

    void setPixel(int8_t x, int8_t y, CRGB color) {
      if (x < 0 || x >= OLB_MATRIX_W || y < 0 || y >= OLB_MATRIX_H) return;
      OLB_LEDS[olb_xy(x, y)] = color;
    }

    // Entprellte Tastenerkennung (Flanke HIGH -> LOW)
    bool checkButton(Button &btn) {
      uint32_t now = millis();
      uint8_t reading = digitalRead(btn.pin);
      bool triggered = false;

      if (reading != btn.lastState) {
        if (reading == LOW && (now - btn.lastDebounceTime > OLB_DEBOUNCE_MS)) {
          triggered = true;
          btn.lastDebounceTime = now;
        }
        btn.lastState = reading;
      }
      return triggered;
    }

    void handleInput() {
      btnUpPressed    = checkButton(btnUp);
      btnDownPressed  = checkButton(btnDown);
      btnLeftPressed  = checkButton(btnLeft);
      btnRightPressed = checkButton(btnRight);

      // Im Spiel: Richtungspufferung mit 180°-Sperre
      if (currentState == STATE_PLAYING) {
        if (btnUpPressed && currentDir != DIR_DOWN) {
          nextDir = DIR_UP;
        } else if (btnDownPressed && currentDir != DIR_UP) {
          nextDir = DIR_DOWN;
        } else if (btnLeftPressed && currentDir != DIR_RIGHT) {
          nextDir = DIR_LEFT;
        } else if (btnRightPressed && currentDir != DIR_LEFT) {
          nextDir = DIR_RIGHT;
        }
      }
    }

    // ============================================================================
    // 9. SPIELLOGIK
    // ============================================================================

    void resetGame() {
      snakeLength = INITIAL_SNAKE_LENGTH;
      currentDir = DIR_RIGHT;
      nextDir = DIR_RIGHT;
      currentSpeedMs = INITIAL_SPEED_MS;
      score = 0;

      // Startposition Kopf: (8,8), Körper: (7,8), (6,8)
      snake[0] = { 8, 8 };
      snake[1] = { 7, 8 };
      snake[2] = { 6, 8 };

      spawnFood();
      lastStepTime = millis();
    }

    void spawnFood() {
      bool onSnake;
      do {
        onSnake = false;
        food.x = random(0, OLB_MATRIX_W);
        food.y = random(0, OLB_MATRIX_H);

        for (uint16_t i = 0; i < snakeLength; i++) {
          if (snake[i].x == food.x && snake[i].y == food.y) {
            onSnake = true;
            break;
          }
        }
      } while (onSnake);
    }

    void updateGame() {
      uint32_t now = millis();
      if (now - lastStepTime < currentSpeedMs) {
        return;
      }
      lastStepTime = now;

      currentDir = nextDir;

      // Neuer Kopf anhand Richtung
      Point newHead = snake[0];
      switch (currentDir) {
        case DIR_UP:    newHead.y--; break;
        case DIR_DOWN:  newHead.y++; break;
        case DIR_LEFT:  newHead.x--; break;
        case DIR_RIGHT: newHead.x++; break;
        default: break;
      }

      // 1. Wand-Kollision
      if (newHead.x < 0 || newHead.x >= OLB_MATRIX_W || newHead.y < 0 || newHead.y >= OLB_MATRIX_H) {
        currentState = STATE_GAMEOVER;
        stateTimer = millis();
        return;
      }

      // 2. Selbst-Kollision (gegen aktuellen Körper ohne Schwanzende)
      for (uint16_t i = 0; i < snakeLength - 1; i++) {
        if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
          currentState = STATE_GAMEOVER;
          stateTimer = millis();
          return;
        }
      }

      // 3. Futter gefressen?
      bool ateFood = (newHead.x == food.x && newHead.y == food.y);

      if (ateFood) {
        score++;
        if (snakeLength < MAX_SNAKE_LENGTH) {
          snakeLength++;
        }
        // Geschwindigkeit steigern
        if (currentSpeedMs > MIN_SPEED_MS + SPEED_DECREMENT_MS) {
          currentSpeedMs -= SPEED_DECREMENT_MS;
        } else {
          currentSpeedMs = MIN_SPEED_MS;
        }
        spawnFood();
      }

      // Körper nachziehen
      for (int16_t i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
      }
      snake[0] = newHead;
    }

    // ============================================================================
    // 10. RENDERING & GRAFIK
    // ============================================================================

    void drawTitle() {
      FastLED.clear();

      // Animiertes Snake-Icon ("S" in der Mitte)
      const Point logoS[] = {
        {8,4}, {7,4}, {6,4}, {5,5}, {5,6}, {6,7}, {7,7}, {8,8}, {9,9}, {9,10}, {8,11}, {7,11}, {6,11}
      };
      for (uint8_t i = 0; i < sizeof(logoS)/sizeof(logoS[0]); i++) {
        setPixel(logoS[i].x, logoS[i].y, CRGB::LimeGreen);
      }
      setPixel(8, 4, CRGB::White); // Kopf des Logo-S

      // Blinkende Richtungsanzeigen an den Kanten (Pulsieren / Takt)
      bool blinkState = (millis() / 350) % 2;
      if (blinkState) {
        // Oben: Blau (D6)
        setPixel(7, 0, CRGB::Blue);
        setPixel(8, 0, CRGB::Blue);

        // Unten: Weiß / Hellgrau (D1)
        setPixel(7, 15, CRGB(180, 180, 180));
        setPixel(8, 15, CRGB(180, 180, 180));

        // Links: Gelb (D5)
        setPixel(0, 7, CRGB::Yellow);
        setPixel(0, 8, CRGB::Yellow);

        // Rechts: Grün (D2)
        setPixel(15, 7, CRGB::Green);
        setPixel(15, 8, CRGB::Green);
      }
    }

    void drawGame() {
      FastLED.clear();

      // Futter (Apfel) - Leicht pulsierend
      uint8_t breath = beatsin8(120, 160, 255);
      setPixel(food.x, food.y, CRGB(breath, 0, 0));

      // Schlangenkörper (Grün)
      for (uint16_t i = 1; i < snakeLength; i++) {
        setPixel(snake[i].x, snake[i].y, CRGB::LimeGreen);
      }

      // Schlangenkopf (Weiß)
      setPixel(snake[0].x, snake[0].y, CRGB::White);
    }

    void drawGameOver() {
      // Rotes Blinken (2 Blitze)
      uint32_t elapsed = millis() - stateTimer;
      if ((elapsed < 300) || (elapsed >= 600 && elapsed < 900)) {
        fill_solid(OLB_LEDS, OLB_NUM_LEDS, CRGB(180, 0, 0));
      } else {
        FastLED.clear();
      }
    }

    void drawDigit(uint8_t startX, uint8_t startY, uint8_t digit, CRGB color) {
      if (digit > 9) return;
      for (uint8_t row = 0; row < 5; row++) {
        uint8_t bits = FONT_3X5[digit][row];
        for (uint8_t col = 0; col < 3; col++) {
          if (bits & (1 << (2 - col))) {
            setPixel(startX + col, startY + row, color);
          }
        }
      }
    }

    void drawScore() {
      FastLED.clear();

      uint16_t displayScore = score;
      if (displayScore > 99) displayScore = 99;

      uint8_t tens = displayScore / 10;
      uint8_t ones = displayScore % 10;

      // Score zentriert anzeigen: Zehner bei X=4, Y=5 | Einer bei X=9, Y=5
      drawDigit(4, 5, tens, CRGB::Cyan);
      drawDigit(9, 5, ones, CRGB::Cyan);
    }
}
