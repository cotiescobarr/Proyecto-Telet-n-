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

// Parpadeo del cursor
unsigned long lastBlinkTime = 0;
const int blinkInterval = 500;
bool blinkState = true;

void setup() {
  // Inicializa botones
  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  pinMode(boton3, INPUT_PULLUP);

  // Inicializa matriz LED
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  Serial.begin(9600);
  resetBoard();
  drawBoard();

  Serial.println("Ta-Te-Ti iniciado con botones físicos.");
}

void loop() {
  // Control de parpadeo del cursor
  unsigned long currentMillis = millis();
  if (currentMillis - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = currentMillis;
    blinkState = !blinkState;
    drawBoard();  // Actualiza visualización
  }

  // Leer botones
  int btn1 = digitalRead(boton1);
  int btn2 = digitalRead(boton2);
  int btn3 = digitalRead(boton3);

  // Eje X
  if (bandera1 && btn1 == LOW) {
    cursorX = (cursorX + 1) % 3;
    Serial.print("CursorX: ");
    Serial.println(cursorX);
    drawBoard();  // Refresca al mover
    delay(150);   // Evita rebotes rápidos
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
        delay(3000);
        resetBoard();
      } else if (isFull()) {
        Serial.println("¡Empate!");
        delay(3000);
        resetBoard();
      } else {
        currentPlayer = 3 - currentPlayer;  // Cambiar jugador
      }
    } else {
      Serial.println("Casilla ocupada. Elige otra.");
    }
    drawBoard();  // Refresca después de selección
    delay(150);   // Evita doble pulsación
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

// ====== FUNCIONES DEL JUEGO =======

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
        // Mostrar cursor parpadeante si la celda está vacía
        lc.setLed(0, y, x, blinkState);
        lc.setLed(0, y, x + 1, blinkState);
        lc.setLed(0, y + 1, x, blinkState);
        lc.setLed(0, y + 1, x + 1, blinkState);
      } else {
        drawSymbol(board[row][col], x, y);
      }
    }
  }
}

void drawSymbol(char symbol, int x, int y) {
  if (symbol == 'X') {
    // Diagonal mini cruz (2 LEDs)
    lc.setLed(0, y, x, true);
    lc.setLed(0, y + 1, x + 1, true);
  } else if (symbol == 'O') {
    // Cuadro (4 LEDs), parpadeante
    lc.setLed(0, y, x, blinkState);
    lc.setLed(0, y, x + 1, blinkState);
    lc.setLed(0, y + 1, x, blinkState);
    lc.setLed(0, y + 1, x + 1, blinkState);
  }
}

void drawGrid() {
  // Verticales
  for (int y = 0; y < 8; y++) {
    lc.setLed(0, y, 2, true);
    lc.setLed(0, y, 5, true);
  }
  // Horizontales
  for (int x = 0; x < 8; x++) {
    lc.setLed(0, 2, x, true);
    lc.setLed(0, 5, x, true);
  }
}

bool checkWin() {
  for (int i = 0; i < 3; i++) {
    if (board[i][0] != ' ' &&
        board[i][0] == board[i][1] &&
        board[i][1] == board[i][2]) return true;

    if (board[0][i] != ' ' &&
        board[0][i] == board[1][i] &&
        board[1][i] == board[2][i]) return true;
  }

  if (board[0][0] != ' ' &&
      board[0][0] == board[1][1] &&
      board[1][1] == board[2][2]) return true;

  if (board[0][2] != ' ' &&
      board[0][2] == board[1][1] &&
      board[1][1] == board[2][0]) return true;

  return false;
}

bool isFull() {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (board[i][j] == ' ') return false;
  return true;
}
