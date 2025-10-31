
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
wins_in_a_row = 0
selected_blocks = []  # bloques acertados que quedan encendidos

# === Fuente 5x8 para GANASTE ===
FONT_ROWS = {
    'G': ["01110","10001","10000","10111","10001","10001","01110","00000"],
    'A': ["01110","10001","10001","11111","10001","10001","10001","00000"],
    'N': ["10001","11001","10101","10011","10001","10001","10001","00000"],
    'S': ["01111","10000","01110","00001","00001","10001","01110","00000"],
    'T': ["11111","00100","00100","00100","00100","00100","00100","00000"],
    'E': ["11111","10000","11110","10000","10000","10000","11111","00000"],
    ' ': ["00000"] * 8
}

def build_columns_from_rows(rows5):
    cols = []
    for c in range(5):
        b = 0
        for r in range(8):
            if rows5[r][c] == '1':
                b |= (1 << (7 - r))
        cols.append(b)
    cols.append(0x00)
    return cols

FONT_COLS = {ch: build_columns_from_rows(rows) for ch, rows in FONT_ROWS.items()}

# === Dibujo de tablero ===
def draw_board_base():
    display.fill(0)
    for y in range(8):
        display.pixel(2, y, 1)
        display.pixel(5, y, 1)
    for x in range(8):
        display.pixel(x, 2, 1)
        display.pixel(x, 5, 1)

def draw_selected_blocks():
    for (x, y) in selected_blocks:
        base_x = x * 3
        base_y = y * 3
        for dx in [0, 1]:
            for dy in [0, 1]:
                display.pixel(base_x + dx, base_y + dy, 1)

def draw_cursor_pixel(x, y):
    base_x = x * 3 + 1
    base_y = y * 3 + 1
    display.pixel(base_x, base_y, 1)

def render_screen(show_cursor=True):
    draw_board_base()
    draw_selected_blocks()
    if show_cursor:
        draw_cursor_pixel(cursor_x, cursor_y)
    display.show()

def flash_block(x, y, delay=0.12):
    base_x = x * 3
    base_y = y * 3
    for dx in [0,1]:
        for dy in [0,1]:
            display.pixel(base_x + dx, base_y + dy, 1)
    display.show()
    time.sleep(delay)
    render_screen(show_cursor=False)

def show_pattern(pattern):
    draw_board_base()
    for (x, y) in pattern:
        base_x = x * 3
        base_y = y * 3
        for dx in [0,1]:
            for dy in [0,1]:
                display.pixel(base_x+dx, base_y+dy, 1)
    display.show()
    time.sleep(0.9)
    render_screen()

def sample_compat(population, k):
    result = []
    temp = population[:]
    while len(result) < k and temp:
        choice = random.choice(temp)
        result.append(choice)
        temp.remove(choice)
    return result

def generate_random_pattern():
    total_blocks = random.randint(2, 4)
    all_blocks = [(x, y) for x in range(3) for y in range(3)]
    return sample_compat(all_blocks, total_blocks)

def show_message_ganaste():
    message = " GANASTE "
    columns = []
    for ch in message:
        columns += FONT_COLS.get(ch, FONT_COLS[' '])

    total = len(columns)
    for offset in range(8, -total - 1, -1):
        display.fill(0)
        for col in range(8):
            src = col - offset
            if 0 <= src < total:
                column = columns[src]
                for row in range(8):
                    if (column >> (7 - row)) & 1:
                        display.pixel(col, row, 1)
        display.show()
        time.sleep(0.08)
    render_screen()

def move_cursor(axis):
    global cursor_x, cursor_y
    if axis == 'x':
        cursor_x = (cursor_x + 1) % grid_size
    elif axis == 'y':
        cursor_y = (cursor_y + 1) % grid_size

# === Bucle principal ===
def memory():
    global selected_blocks, wins_in_a_row

    while True:
        pattern = generate_random_pattern()
        selected_blocks = []  # limpiar encendidos al nuevo patrón
        show_pattern(pattern)
        user_hits = []

        while True:
            render_screen(show_cursor=True)

            if not btn_x.value():
                time.sleep(0.12)
                move_cursor('x')
                while not btn_x.value(): pass

            if not btn_y.value():
                time.sleep(0.12)
                move_cursor('y')
                while not btn_y.value(): pass

            if not btn_sel.value():
                pos = (cursor_x, cursor_y)
                if pos in pattern and pos not in user_hits:
                    user_hits.append(pos)
                    selected_blocks.append(pos)  # ahora queda encendido
                    render_screen(show_cursor=False)
                    flash_block(cursor_x, cursor_y, delay=0.12)

                    if set(user_hits) == set(pattern):
                        wins_in_a_row += 1
                        break
                else:
                    wins_in_a_row = 0
                    for _ in range(2):
                        display.fill(1)
                        display.show()
                        time.sleep(0.12)
                        render_screen()
                        time.sleep(0.08)
                    break

                while not btn_sel.value(): pass

        if wins_in_a_row >= 3:
            show_message_ganaste()
            wins_in_a_row = 0

        time.sleep(0.25)

# === Inicio ===
render_screen()
time.sleep(0.5)
memory()
