#include <LedControl.h>

// Pines para la matriz LED: DIN=12, CLK=11, CS=10
LedControl lc = LedControl(12, 11, 10, 1);

// Pines de botones
const int boton1 = 2;  // Eje X
const int boton2 = 3;  // Selección
const int boton3 = 4;  // Eje Y

// Estados anteriores de botones (para detectar flanco descendente)
bool bandera1 = false;
bool bandera2 = false;
bool bandera3 = false;

// Lógica del juego
char board[3][3];
int currentPlayer = 1;
int cursorX = 0;
int cursorY = 0;

// Parpadeo del cursor y jugador 2 (O)
unsigned long lastBlinkTimeCursor = 0;
const int blinkIntervalCursor = 200; // blink del puntito cursor

unsigned long lastBlinkTimePlayer2 = 0;
const int blinkIntervalPlayer2 = 500; // blink de jugador 2 

bool blinkStateCursor = true;
bool blinkStatePlayer2 = true;

// ================== LETRAS GANA J1 / GANA J2 ==================

byte letrasGANA_J1[6][8] = {
  {
    B00111100,
    B01000010,
    B10000000,
    B10011110,
    B10000010,
    B10000010,
    B01000010,
    B00111100
  },
  {
    B00111000,
    B01000100,
    B10000010,
    B10000010,
    B11111110,
    B10000010,
    B10000010,
    B10000010
  },
  {
    B10000010,
    B11000010,
    B10100010,
    B10010010,
    B10001010,
    B10000110,
    B10000010,
    B10000010
  },
  {
    B00111000,
    B01000100,
    B10000010,
    B10000010,
    B11111110,
    B10000010,
    B10000010,
    B10000010
  },
  {
    B00011110,
    B00000100,
    B00000100,
    B00000100,
    B00000100,
    B10000100,
    B01001000,
    B00110000
  },
  {
    B00010000,
    B00110000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00111000
  }
};

byte letrasGANA_J2[6][8] = {
  {
    B00111100,
    B01000010,
    B10000000,
    B10011110,
    B10000010,
    B10000010,
    B01000010,
    B00111100
  },
  {
    B00111000,
    B01000100,
    B10000010,
    B10000010,
    B11111110,
    B10000010,
    B10000010,
    B10000010
  },
  {
    B10000010,
    B11000010,
    B10100010,
    B10010010,
    B10001010,
    B10000110,
    B10000010,
    B10000010
  },
  {
    B00111000,
    B01000100,
    B10000010,
    B10000010,
    B11111110,
    B10000010,
    B10000010,
    B10000010
  },
  {
    B00011110,
    B00000100,
    B00000100,
    B00000100,
    B00000100,
    B10000100,
    B01001000,
    B00110000
  },
  {
    B00111100,
    B01000010,
    B00000010,
    B00000100,
    B00001000,
    B00010000,
    B00100000,
    B01111110
  }
};

void mostrarLetra(byte letra[8]) {
  lc.clearDisplay(0);
  for (int fila = 0; fila < 8; fila++) {
    for (int col = 0; col < 8; col++) {
      bool bit = bitRead(letra[fila], 7 - col);
      lc.setLed(0, fila, col, bit);
    }
  }
}

void mostrarGanador(int jugador) {
  if (jugador == 1) {
    for (int i = 0; i < 6; i++) {
      mostrarLetra(letrasGANA_J1[i]);
      delay(600);
    }
  } else {
    for (int i = 0; i < 6; i++) {
      mostrarLetra(letrasGANA_J2[i]);
      delay(600);
    }
  }
}

// ===================== SETUP =====================

void setup() {
  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  pinMode(boton3, INPUT_PULLUP);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  Serial.begin(9600);
  resetBoard();
  drawBoard();

  Serial.println("Ta-Te-Ti iniciado con botones físicos.");
}

// ===================== LOOP =====================

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastBlinkTimeCursor >= blinkIntervalCursor) {
    lastBlinkTimeCursor = currentMillis;
    blinkStateCursor = !blinkStateCursor;
    drawBoard();
  }

  if (currentMillis - lastBlinkTimePlayer2 >= blinkIntervalPlayer2) {
    lastBlinkTimePlayer2 = currentMillis;
    blinkStatePlayer2 = !blinkStatePlayer2;
    // No hace falta llamar a drawBoard aquí porque ya lo hace el cursor, pero no molesta si lo haces
  }

  int btn1 = digitalRead(boton1);
  int btn2 = digitalRead(boton2);
  int btn3 = digitalRead(boton3);

  // Eje X
  if (bandera1 && btn1 == LOW) {
    cursorX = (cursorX + 1) % 3;
    Serial.print("CursorX: ");
    Serial.println(cursorX);
    drawBoard();
    delay(150);
  }

  // Seleccionar
  if (bandera2 && btn2 == LOW) {
    if (board[cursorY][cursorX] == ' ') {
      board[cursorY][cursorX] = (currentPlayer == 1) ? 'X' : 'O';
      Serial.print("Jugador ");
      Serial.print(currentPlayer);
      Serial.print(" jugó en ");
      Serial.print(cursorY);
      Serial.print(", ");
      Serial.println(cursorX);

      if (checkWin()) {
        Serial.print("¡Jugador ");
        Serial.print(currentPlayer);
        Serial.println(" ha ganado!");

        mostrarGanador(currentPlayer);

        delay(2000);
        resetBoard();
      } else if (isFull()) {
        Serial.println("¡Empate!");
        delay(2000);
        resetBoard();
      } else {
        currentPlayer = 3 - currentPlayer;
      }
    } else {
      Serial.println("Casilla ocupada. Elige otra.");
    }
    drawBoard();
    delay(150);
  }

  // Eje Y
  if (bandera3 && btn3 == LOW) {
    cursorY = (cursorY + 1) % 3;
    Serial.print("CursorY: ");
    Serial.println(cursorY);
    drawBoard();
    delay(150);
  }

  // Actualizar banderas
  bandera1 = btn1;
  bandera2 = btn2;
  bandera3 = btn3;
}

// ===================== FUNCIONES =====================

void resetBoard() {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      board[i][j] = ' ';
  currentPlayer = 1;
  cursorX = 0;
  cursorY = 0;
  drawBoard();
}

void drawBoard() {
  lc.clearDisplay(0);
  drawGrid();
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int x = col * 3;
      int y = row * 3;

      if (row == cursorY && col == cursorX && board[row][col] == ' ') {
        // Puntito parpadeante cursor
        if (blinkStateCursor) {
          lc.setLed(0, y, x, true);
        }
      } else {
        drawSymbol(board[row][col], x, y);
      }
    }
  }
}

void drawSymbol(char symbol, int x, int y) {
  if (symbol == 'X') {
    // X siempre encendido, sin parpadeo
    lc.setLed(0, y, x, true);
    lc.setLed(0, y + 1, x + 1, true);
  } else if (symbol == 'O') {
    // O parpadea con blinkStatePlayer2
    lc.setLed(0, y, x, blinkStatePlayer2);
    lc.setLed(0, y, x + 1, blinkStatePlayer2);
    lc.setLed(0, y + 1, x, blinkStatePlayer2);
    lc.setLed(0, y + 1, x + 1, blinkStatePlayer2);
  }
}

void drawGrid() {
  for (int i = 0; i < 8; i++) {
    lc.setLed(0, 2, i, true);
    lc.setLed(0, 5, i, true);
    lc.setLed(0, i, 2, true);
    lc.setLed(0, i, 5, true);
  }
}

bool checkWin() {
  for (int i = 0; i < 3; i++) {
    if (board[i][0] != ' ' &&
        board[i][0] == board[i][1] &&
        board[i][1] == board[i][2]) {
      return true;
    }
    if (board[0][i] != ' ' &&
        board[0][i] == board[1][i] &&
        board[1][i] == board[2][i]) {
      return true;
    }
  }

  if (board[0][0] != ' ' &&
      board[0][0] == board[1][1] &&
      board[1][1] == board[2][2]) {
    return true;
  }
  if (board[0][2] != ' ' &&
      board[0][2] == board[1][1] &&
      board[1][1] == board[2][0]) {
    return true;
  }

  return false;
}

bool isFull() {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (board[i][j] == ' ')
        return false;
  return true;
}

