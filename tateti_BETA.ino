#include <LedControl.h>

#define DIN 12
#define CS 10
#define CLK 11

LedControl lc = LedControl(DIN, CLK, CS, 1);

const int size = 3;
int currentPattern[size][size];
int user[size][size];
int streak = 0;

// Lista de patrones predefinidos
const int numPatterns = 6;
int patterns[numPatterns][size][size] = {
  { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0 } },
  { { 0, 1, 0 }, { 1, 0, 1 }, { 0, 0, 0 } },
  { { 1, 1, 0 }, { 0, 1, 0 }, { 0, 0, 1 } },
  { { 0, 0, 1 }, { 1, 1, 0 }, { 0, 1, 0 } },
  { { 1, 0, 1 }, { 0, 1, 0 }, { 1, 0, 0 } },
  { { 0, 1, 0 }, { 1, 0, 0 }, { 0, 1, 1 } }
};

// Patrón 8x8 para "GANASTE"
byte ganastePattern[8] = {
  B11101110,  // G A N A S T E
  B10001001,
  B10101101,
  B10101011,
  B10101101,
  B10001001,
  B11101110,
  B00000000
};

void setup() {
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  randomSeed(analogRead(0));
  Serial.println("Juego MEMORY 3x3 - Arduino");
  mostrarGanaste();
  mostrarTablero();
  delay(2000);
  nuevaRonda();
}

void loop() {
  if (Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    int spaceIndex = line.indexOf(' ');
    if (spaceIndex == -1) {
      Serial.println("Formato inválido. Usa: fila columna");
      return;
    }

    String first = line.substring(0, spaceIndex);
    String second = line.substring(spaceIndex + 1);

    if (!isNumber(first) || !isNumber(second)) {
      Serial.println("Entrada inválida. Usa números 0-2 separados por espacio");
      return;
    }

    int fila = first.toInt();
    int col = second.toInt();

    if (fila < 0 || fila >= size || col < 0 || col >= size) {
      Serial.println("Movimiento fuera de rango. Usa valores 0-2");
      return;
    }

    if (user[fila][col] == 1) {
      Serial.println("Ya ingresaste esa coordenada. Intenta otra.");
      return;
    }

    // Verificar inmediatamente si la coordenada es correcta
    if (currentPattern[fila][col] == 1) {
      user[fila][col] = 1;
      Serial.print("¡Correcto! Casilla (");
      Serial.print(fila);
      Serial.print(", ");
      Serial.print(col);
      Serial.println(") marcada.");
    } else {
      Serial.print("¡Error! Las casillas correctas eran: ");
      for (int f = 0; f < size; f++)
        for (int c = 0; c < size; c++)
          if (currentPattern[f][c] == 1 && user[f][c] != 1)
            Serial.print("(" + String(f) + "," + String(c) + ") ");
      Serial.println("\nRacha reiniciada.");
      streak = 0;
      delay(2000);
      nuevaRonda();
      return;
    }

    // Si completó todas las casillas del patrón correctamente
    if (jugadaCompleta()) {
      streak++;
      Serial.print("¡Correcto! Racha: ");
      Serial.println(streak);
      if (streak == 3) {
        mostrarGanaste();
        streak = 0;
      }
      delay(2000);
      nuevaRonda();
    }
  }
}

// ---------- FUNCIONES ----------

void mostrarTablero() {
  lc.clearDisplay(0);
  for (int i = 0; i < 8; i++) {
    lc.setLed(0, 2, i, true);
    lc.setLed(0, 5, i, true);
    lc.setLed(0, i, 2, true);
    lc.setLed(0, i, 5, true);
  }
}

void nuevaRonda() {
  limpiarMatrices();
  mostrarTablero();

  int idx = random(0, numPatterns);
  for (int f = 0; f < size; f++)
    for (int c = 0; c < size; c++)
      currentPattern[f][c] = patterns[idx][f][c];

  // Mostrar patrón por 5 segundos
  for (int f = 0; f < size; f++)
    for (int c = 0; c < size; c++)
      if (currentPattern[f][c] == 1) encenderCelda(f, c, true);

  Serial.println("Patrón generado. Memoriza...");
  delay(5000);

  // Apagar patrón dejando tablero visible
  for (int f = 0; f < size; f++)
    for (int c = 0; c < size; c++)
      if (currentPattern[f][c] == 1) encenderCelda(f, c, false);

  Serial.println("Ingresa las coordenadas (fila columna) una por una:");
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

bool isNumber(String str) {
  for (unsigned int i = 0; i < str.length(); i++)
    if (!isDigit(str[i])) return false;
  return true;
}

// ---------- MOSTRAR GANASTE ----------
void mostrarGanaste() {
  lc.clearDisplay(0);
  Serial.println(" GANASTE ");

  for (int fila = 0; fila < 8; fila++) {
    byte row = ganastePattern[fila];
    for (int col = 0; col < 8; col++) {
      bool bit = bitRead(row, col);  // leer cada bit de izquierda a derecha
      lc.setLed(0, fila, col, bit);
    }
  }

  delay(2000);       // mostrar 2 segundos
  mostrarTablero();  // volver al tablero
}




//(0,0) (0,1) (0,2)
//(1,0) (1,1) (1,2)
//(2,0) (2,1) (2,2)
