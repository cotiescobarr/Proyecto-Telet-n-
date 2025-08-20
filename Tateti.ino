#include <LedControl.h>

// Pines: DIN=12, CLK=11, CS=10
LedControl lc = LedControl(12, 11, 10, 1);

char board[3][3];
int currentPlayer = 1;
unsigned long lastBlinkTime = 0;
bool blinkState = true;
const int blinkInterval = 500;

void setup() {
  Serial.begin(9600);

  // Limpia cualquier dato basura del buffer serial
  while (Serial.available() > 0) {
    Serial.read();
  }

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  resetBoard();
  drawBoard();

  Serial.println("Bienvenido al Ta-Te-Ti en LED!");
  Serial.println("Jugador 1 = X | Jugador 2 = O");
  Serial.println("Ingrese coordenadas de la casilla (ej: 0 1):");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = currentMillis;
    blinkState = !blinkState;
    drawBoard();
  }

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  // Leer línea completa
    input.trim();  // Quitar espacios y saltos extra

    if (input.length() == 0) return;  // Ignorar líneas vacías

    int spaceIndex = input.indexOf(' ');
    if (spaceIndex == -1) {
      Serial.println("Entrada inválida. Usa formato: fila columna");
      return;
    }

    String rowStr = input.substring(0, spaceIndex);
    String colStr = input.substring(spaceIndex + 1);

    int row = rowStr.toInt();
    int col = colStr.toInt();

    if (row < 0 || row > 2 || col < 0 || col > 2) {
      Serial.println("Movimiento inválido. Usa valores entre 0 y 2.");
      return;
    }

    if (board[row][col] == ' ') {
      board[row][col] = (currentPlayer == 1) ? 'X' : 'O';
      drawBoard();

      if (checkWin()) {
        Serial.print("Jugador ");
        Serial.print(currentPlayer);
        Serial.println(" ha ganado!");
        delay(3000);
        resetBoard();
        drawBoard();
      } else if (isFull()) {
        Serial.println("¡Empate!");
        delay(3000);
        resetBoard();
        drawBoard();
      } else {
        currentPlayer = 3 - currentPlayer;
        Serial.println("Siguiente movimiento:");
      }
    } else {
      Serial.println("Casilla ocupada. Intenta otra.");
    }
  }
}

void resetBoard() {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      board[i][j] = ' ';
  currentPlayer = 1;
}

void drawBoard() {
  lc.clearDisplay(0);
  drawGrid();
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int x = col * 3;
      int y = row * 3;
      drawSymbol(board[row][col], x, y);
    }
  }
}

void drawSymbol(char symbol, int x, int y) {
  if (symbol == 'X') {
    // Mini diagonal: dos LEDs
    lc.setLed(0, y, x, true);
    lc.setLed(0, y + 1, x + 1, true);
  } else if (symbol == 'O') {
    // Parpadeo con 4 LEDs formando cuadro
    lc.setLed(0, y, x, blinkState);
    lc.setLed(0, y, x + 1, blinkState);
    lc.setLed(0, y + 1, x, blinkState);
    lc.setLed(0, y + 1, x + 1, blinkState);
  }
}

void drawGrid() {
  // Líneas verticales divisorias
  for (int y = 0; y < 8; y++) {
    lc.setLed(0, y, 2, true);
    lc.setLed(0, y, 5, true);
  }
  // Líneas horizontales divisorias
  for (int x = 0; x < 8; x++) {
    lc.setLed(0, 2, x, true);
    lc.setLed(0, 5, x, true);
  }
}

bool checkWin() {
  for (int i = 0; i < 3; i++) {
    if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
      return true;
    if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
      return true;
  }

  if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
    return true;
  if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
    return true;

  return false;
}

bool isFull() {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (board[i][j] == ' ')
        return false;
  return true;
}

