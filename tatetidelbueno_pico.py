# tateti_pico.py
# Adaptación del juego Tatetí (Ta-Te-Ti) para Raspberry Pi Pico + MAX7219
# Usa 3 botones: eje X, eje Y y seleccionar

from machine import Pin, SPI
import time
import max7219

# ==== CONFIGURACIÓN DE PINES ====
BOTON_X = 14      # Mover en eje X
BOTON_SEL = 15    # Seleccionar
BOTON_Y = 16      # Mover en eje Y

# Configuración del display MAX7219 (SPI0)
SPI_SCK = 2
SPI_MOSI = 3
CS_PIN = 5

# ==== INICIALIZACIÓN DE HARDWARE ====
spi = SPI(0, sck=Pin(SPI_SCK), mosi=Pin(SPI_MOSI))
cs = Pin(CS_PIN, Pin.OUT)
display = max7219.Matrix8x8(spi, cs, 1)
display.brightness(5)

btn_x = Pin(BOTON_X, Pin.IN, Pin.PULL_UP)
btn_y = Pin(BOTON_Y, Pin.IN, Pin.PULL_UP)
btn_sel = Pin(BOTON_SEL, Pin.IN, Pin.PULL_UP)

# ==== VARIABLES DE JUEGO ====
tablero = [[' ' for _ in range(3)] for _ in range(3)]
turno = 'X'
cursor = [0, 0]  # [fila, columna]
fin_juego = False

# Para el parpadeo de j2
ultimo_parpadeo = time.ticks_ms()
intervalo_parpadeo = 500  # tiempo en ms
mostrar_j2 = True

# ==== FUNCIONES ====

def dibujar_tablero():
    display.fill(0)
    # Líneas separadoras del tablero
    for i in range(8):
        if i in (2, 5):
            for j in range(8):
                display.pixel(i, j, 1)
                display.pixel(j, i, 1)
    # Fichas
    for y in range(3):
        for x in range(3):
            px = x * 3
            py = y * 3
            if tablero[y][x] == 'X':
                display.pixel(px, py, 1)
                display.pixel(px+2, py+2, 1)
                display.pixel(px+2, py, 1)
                display.pixel(px, py+2, 1)
                display.pixel(px+1, py+1, 1)
            elif tablero[y][x] == 'O' and mostrar_j2:
                for dx in range(3):
                    for dy in range(3):
                        display.pixel(px+dx, py+dy, 1)
    # Cursor
    cx = cursor[1] * 3 + 1
    cy = cursor[0] * 3 + 1
    display.pixel(cx, cy, not display.pixel(cx, cy))
    display.show()

def actualizar_parpadeo():
    global mostrar_j2, ultimo_parpadeo
    ahora = time.ticks_ms()
    if time.ticks_diff(ahora, ultimo_parpadeo) > intervalo_parpadeo:
        mostrar_j2 = not mostrar_j2
        ultimo_parpadeo = ahora
        dibujar_tablero()

def mover_x():
    cursor[1] = (cursor[1] + 1) % 3
    dibujar_tablero()

def mover_y():
    cursor[0] = (cursor[0] + 1) % 3
    dibujar_tablero()

def marcar():
    global turno, fin_juego
    if tablero[cursor[0]][cursor[1]] == ' ':
        tablero[cursor[0]][cursor[1]] = turno
        if hay_ganador(turno):
            mostrar_mensaje(turno + " gana!")
            reiniciar()
        elif tablero_lleno():
            mostrar_mensaje("Empate!")
            reiniciar()
        else:
            turno = 'O' if turno == 'X' else 'X'
    dibujar_tablero()

def tablero_lleno():
    for fila in tablero:
        if ' ' in fila:
            return False
    return True

def hay_ganador(jugador):
    for i in range(3):
        if all(tablero[i][j] == jugador for j in range(3)): return True
        if all(tablero[j][i] == jugador for j in range(3)): return True
    if all(tablero[i][i] == jugador for i in range(3)): return True
    if all(tablero[i][2 - i] == jugador for i in range(3)): return True
    return False

def mostrar_mensaje(texto):
    column = (len(texto) * 8)
    for x in range(32, -column, -1):
        display.fill(0)
        display.text(texto, x, 0, 1)
        display.show()
        time.sleep(0.1)

def reiniciar():
    global tablero, turno, cursor, fin_juego
    tablero = [[' ' for _ in range(3)] for _ in range(3)]
    turno = 'X'
    cursor = [0, 0]
    fin_juego = False
    dibujar_tablero()

# ==== PROGRAMA PRINCIPAL ====
dibujar_tablero()

print("TATETI iniciado. Botones: X=14, SEL=15, Y=16")

while True:
    actualizar_parpadeo()
    
    if not btn_x.value():
        mover_x()
        time.sleep(0.25)
    if not btn_y.value():
        mover_y()
        time.sleep(0.25)
    if not btn_sel.value():
        if fin_juego:
            reiniciar()
        else:
            marcar()
        time.sleep(0.25)
