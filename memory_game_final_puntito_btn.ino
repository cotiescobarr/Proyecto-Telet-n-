#include <LedControl.h>

// Pines matriz 8x8 (MAX7219)
#define DIN 12
#define CS 10
#define CLK 11
LedControl lc = LedControl(DIN, CLK, CS, 1);

// Pines de botones
const int botonX = 2;  // mover en X
const int botonSel = 3; // seleccionar
const int botonY = 4;  // mover en Y

bool banderaX = false;
bool banderaSel = false;
bool banderaY = false;

// Juego
const int size = 3;
int currentPattern[size][size];
int user[size][size];
int streak = 0;

int cursorX = 0;
int cursorY = 0;

// Parpadeo del cursor
unsigned long lastBlink = 0;
bool cursorOn = true;

// Lista de patrones predefinidos
const int numPatterns = 25; // ahora 25 patrones
int patterns[numPatterns][size][size] = {
  // Patrones básicos
  { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0 } },
  { { 0, 1, 0 }, { 1, 0, 1 }, { 0, 0, 0 } },
  { { 1, 1, 0 }, { 0, 1, 0 }, { 0, 0, 1 } },
  { { 0, 0, 1 }, { 1, 1, 0 }, { 0, 1, 0 } },
  { { 1, 0, 1 }, { 0, 1, 0 }, { 1, 0, 0 } },
  { { 0, 1, 0 }, { 1, 0, 0 }, { 0, 1, 1 } },
  // Cruces y diagonales
  { { 0, 1, 0 }, { 1, 1, 1 }, { 0, 1, 0 } },
  { { 1, 0, 1 }, { 0, 1, 0 }, { 1, 0, 1 } },
  { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } },
  { { 0, 0, 1 }, { 0, 1, 0 }, { 1, 0, 0 } },
  // Líneas
  { { 1, 1, 1 }, { 0, 0, 0 }, { 0, 0, 0 } },
  { { 0, 0, 0 }, { 1, 1, 1 }, { 0, 0, 0 } },
  { { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 } },
  { { 1, 0, 0 }, { 1, 0, 0 }, { 1, 0, 0 } },
  { { 0, 1, 0 }, { 0, 1, 0 }, { 0, 1, 0 } },
  { { 0, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 } },
  // Esquinas
  { { 1, 0, 1 }, { 0, 0, 0 }, { 1, 0, 1 } },
  { { 1, 1, 0 }, { 1, 0, 0 }, { 0, 0, 0 } },
  { { 0, 1, 1 }, { 0, 0, 1 }, { 0, 0, 0 } },
  { { 0, 0, 0 }, { 1, 0, 1 }, { 1, 1, 0 } },
  { { 0, 0, 0 }, { 0, 1, 1 }, { 0, 1, 1 } },
  // Aleatorios complejos
  { { 1, 0, 0 }, { 1, 1, 0 }, { 0, 1, 1 } },
  { { 0, 1, 0 }, { 1, 0, 1 }, { 1, 1, 0 } },
  { { 1, 0, 1 }, { 1, 1, 0 }, { 0, 1, 0 } },
  { { 0, 1, 1 }, { 1, 0, 0 }, { 1, 0, 1 } }
};


// Patrón “GANASTE”
// Patrón “GANASTE”
// Letras "GANASTE" en 8x8
byte letras[7][8] = {
  // G
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
  // A
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
  // N
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
  // A
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
  // S
  {
    B01111110,
    B10000000,
    B10000000,
    B01111100,
    B00000010,
    B00000010,
    B10000010,
    B01111100
  },
  // T
  {
    B11111110,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000
  },
  // E
  {
    B11111110,
    B10000000,
    B10000000,
    B11111100,
    B10000000,
    B10000000,
    B10000000,
    B11111110
  }
};


void setup() {
  pinMode(botonX, INPUT_PULLUP);
  pinMode(botonSel, INPUT_PULLUP);
  pinMode(botonY, INPUT_PULLUP);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  Serial.begin(9600);
  Serial.println("Juego MEMORY 3x3 con botones");

  randomSeed(analogRead(0));
  nuevaRonda();
}

void loop() {
  // Parpadeo cursor
  if (millis() - lastBlink > 300) {
    cursorOn = !cursorOn;
    lastBlink = millis();
    drawBoard();
  }

  // Leer botones
  int btnX = digitalRead(botonX);
  int btnSel = digitalRead(botonSel);
  int btnY = digitalRead(botonY);

  // Mover X
  if (banderaX && btnX == LOW) {
    cursorX = (cursorX + 1) % 3;
    drawBoard();
    delay(150);
  }

  // Seleccionar
  if (banderaSel && btnSel == LOW) {
    handleSelection();
    drawBoard();
    delay(150);
  }

  // Mover Y
  if (banderaY && btnY == LOW) {
    cursorY = (cursorY + 1) % 3;
    drawBoard();
    delay(150);
  }

  // Actualizar banderas
  banderaX = btnX;
  banderaSel = btnSel;
  banderaY = btnY;
}

// ===== FUNCIONES =====

void nuevaRonda() {
  limpiarMatrices();
  drawBoard();

  int idx = random(0, numPatterns);
  for (int f = 0; f < size; f++)
    for (int c = 0; c < size; c++)
      currentPattern[f][c] = patterns[idx][f][c];

  // Mostrar patrón por 3 segundos
  for (int f = 0; f < size; f++)
    for (int c = 0; c < size; c++)
      if (currentPattern[f][c] == 1) encenderCelda(f, c, true);

  Serial.println("Patrón generado. Memoriza...");
  delay(3000);

  // Apagar patrón
  for (int f = 0; f < size; f++)
    for (int c = 0; c < size; c++)
      if (currentPattern[f][c] == 1) encenderCelda(f, c, false);

  Serial.println("Selecciona con los botones!");
}

void handleSelection() {
  if (user[cursorY][cursorX] == 1) {
    Serial.println("Ya elegiste esa casilla.");
    return;
  }

  if (currentPattern[cursorY][cursorX] == 1) {
    user[cursorY][cursorX] = 1;
    Serial.println("¡Correcto!");
  } else {
    Serial.println("¡Error! Racha reiniciada.");
    streak = 0;
    delay(2000);
    nuevaRonda();
    return;
  }

  if (jugadaCompleta()) {
    streak++;
    Serial.print("¡Completaste el patrón! Racha: ");
    Serial.println(streak);

    if (streak == 3) {
      mostrarGanasteScroll();
      streak = 0;
    }
    delay(2000);
    nuevaRonda();
  }
}

void drawBoard() {
  lc.clearDisplay(0);
  drawGrid();

  // Dibujar jugadas confirmadas
  for (int f = 0; f < size; f++) {
    for (int c = 0; c < size; c++) {
      if (user[f][c] == 1) {
        encenderCelda(f, c, true); // bloque 2x2
      }
    }
  }

  // Dibujar cursor parpadeante
  if (cursorOn) {
    drawCursor(cursorX, cursorY);
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

void drawCursor(int col, int fila) {
  int x = col * 3;
  int y = fila * 3;
  lc.setLed(0, y, x, true); // puntito central
}

void encenderCelda(int fila, int col, bool estado) {
  int startRow = fila * 3;
  int startCol = col * 3;
  for (int r = 0; r < 2; r++)
    for (int c = 0; c < 2; c++)
      lc.setLed(0, startRow + r, startCol + c, estado);
}

bool jugadaCompleta() {
  for (int f = 0; f < size; f++)
    for (int c = 0; c < size; c++)
      if (currentPattern[f][c] == 1 && user[f][c] != 1)
        return false;
  return true;
}

void limpiarMatrices() {
  for (int f = 0; f < size; f++)
    for (int c = 0; c < size; c++) {
      user[f][c] = 0;
      currentPattern[f][c] = 0;
    }
}

void mostrarLetra(byte letra[8]) {
  lc.clearDisplay(0);
  for (int fila = 0; fila < 8; fila++) {
    for (int col = 0; col < 8; col++) {
      bool bit = bitRead(letra[fila], 7 - col);
      lc.setLed(0, fila, col, bit);
    }
  }
}
void mostrarGanasteScroll() {
  for (int i = 0; i < 7; i++) { // 7 letras
    mostrarLetra(letras[i]);
    delay(600); // tiempo de cada letra
  }
}
