# memory_game_infinito.py
# Raspberry Pi Pico - Memory Game infinito con mensaje GANASTE
# Cursor: 1 pixel, patrones aleatorios, 3 aciertos consecutivos para ganar
# Botones: X=GP14, Select=GP15, Y=GP16

from machine import Pin, SPI
import time
import random
from max7219 import Matrix8x8

# === Configuración de pines ===
spi = SPI(0, baudrate=10000000, polarity=0, phase=0,
          sck=Pin(2), mosi=Pin(3))
cs = Pin(5, Pin.OUT)

# Botones físicos
btn_x = Pin(14, Pin.IN, Pin.PULL_UP)
btn_sel = Pin(15, Pin.IN, Pin.PULL_UP)
btn_y = Pin(16, Pin.IN, Pin.PULL_UP)

# === Inicializar matriz ===
display = Matrix8x8(spi, cs, 1)
display.brightness(5)
display.fill(0)
display.show()

# === Variables ===
cursor_x = 0
cursor_y = 0
grid_size = 3
player_sequence = []
wins_in_a_row = 0

# === Dibujo del tablero ===
def draw_board():
    """Dibuja líneas divisorias del tablero 3x3"""
    display.fill(0)
    for y in range(8):
        display.pixel(2, y, 1)
        display.pixel(5, y, 1)
    for x in range(8):
        display.pixel(x, 2, 1)
        display.pixel(x, 5, 1)
    display.show()

def draw_cursor(x, y):
    """Dibuja cursor como un solo pixel"""
    draw_board()
    base_x = x * 3 + 1
    base_y = y * 3 + 1
    display.pixel(base_x, base_y, 1)
    display.show()

def flash_block(x, y, delay=0.4):
    """Destella bloque completo 2x2"""
    draw_board()
    base_x = x * 3
    base_y = y * 3
    for dx in [0,1]:
        for dy in [0,1]:
            display.pixel(base_x+dx, base_y+dy, 1)
    display.show()
    time.sleep(delay)
    draw_board()
    display.show()

def show_pattern(x, y):
    """Muestra un bloque completo 2x2 como patrón"""
    draw_board()
    base_x = x * 3
    base_y = y * 3
    for dx in [0,1]:
        for dy in [0,1]:
            display.pixel(base_x+dx, base_y+dy, 1)
    display.show()
    time.sleep(0.6)
    draw_board()
    display.show()

def generate_random_block():
    """Genera una coordenada aleatoria 3x3"""
    return (random.randint(0,2), random.randint(0,2))

def show_message_ganaste():
    """Muestra 'GANASTE' en la matriz"""
    display.fill(0)
    # Representación 8x8 simple de GANASTE
    # G A N A S T E (cada letra ocupa 1 columna aprox)
    ganaste = [
        0b01111110,  # G
        0b01001000,  # A
        0b01010100,  # N
        0b01001000,  # A
        0b01111100,  # S
        0b01010110,  # T
        0b01111010,  # E
        0b00000000
    ]
    for x in range(8):
        for y in range(8):
            if (ganaste[x] >> (7-y)) & 1:
                display.pixel(x, y, 1)
    display.show()
    time.sleep(2)
    draw_board()
    display.show()

def move_cursor(axis):
    """Mueve el cursor entre los 3x3 bloques"""
    global cursor_x, cursor_y
    if axis == 'x':
        cursor_x = (cursor_x + 1) % grid_size
    elif axis == 'y':
        cursor_y = (cursor_y + 1) % grid_size
    draw_cursor(cursor_x, cursor_y)

# === Juego principal infinito ===
draw_board()
draw_cursor(cursor_x, cursor_y)
time.sleep(1)

while True:
    # Generar patrón aleatorio
    pattern = generate_random_block()
    show_pattern(*pattern)

    # Esperar input del jugador
    correct = False
    while not correct:
        if not btn_x.value():
            time.sleep(0.15)
            move_cursor('x')
            while not btn_x.value():
                pass
        if not btn_y.value():
            time.sleep(0.15)
            move_cursor('y')
            while not btn_y.value():
                pass
        if not btn_sel.value():
            if (cursor_x, cursor_y) == pattern:
                flash_block(cursor_x, cursor_y, delay=0.2)
                wins_in_a_row += 1
                correct = True
            else:
                wins_in_a_row = 0
                correct = True
            while not btn_sel.value():
                pass

    # Verificar si ganó
    if wins_in_a_row >= 3:
        show_message_ganaste()
        wins_in_a_row = 0
