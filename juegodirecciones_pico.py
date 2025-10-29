# simon_flechas_pico.py
# Juego con flechas: derecha, izquierda, arriba
# Mantiene misma configuración de pines y SPI solicitada

from machine import Pin, SPI
import time
import random
from max7219 import Matrix8x8

# === Configuración de pines ===
spi = SPI(0, baudrate=10000000, polarity=0, phase=0, sck=Pin(2), mosi=Pin(3))
cs = Pin(5, Pin.OUT)

btn_x = Pin(14, Pin.IN, Pin.PULL_UP)  # derecha
btn_sel = Pin(15, Pin.IN, Pin.PULL_UP) # arriba
btn_y = Pin(16, Pin.IN, Pin.PULL_UP) # izquierda

# === Matriz LED ===
display = Matrix8x8(spi, cs, 1)
display.brightness(5)

# === Flechas en 8x8 ===
ARROWS = {
    "UP": [
        [0,0,0,1,1,0,0,0],
        [0,0,1,1,1,1,0,0],
        [0,1,1,1,1,1,1,0],
        [0,0,0,1,1,0,0,0],
        [0,0,0,1,1,0,0,0],
        [0,0,0,1,1,0,0,0],
        [0,0,0,1,1,0,0,0],
        [0,0,0,1,1,0,0,0]
    ],
    "RIGHT": [
        [0,0,0,0,0,0,0,0],
        [0,0,0,0,0,1,0,0],
        [0,0,0,0,0,1,1,0],
        [1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1],
        [0,0,0,0,0,1,1,0],
        [0,0,0,0,0,1,0,0],
        [0,0,0,0,0,0,0,0]
    ],
    "LEFT": [
        [0,0,0,0,0,0,0,0],
        [0,0,1,0,0,0,0,0],
        [0,1,1,0,0,0,0,0],
        [1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1],
        [0,1,1,0,0,0,0,0],
        [0,0,1,0,0,0,0,0],
        [0,0,0,0,0,0,0,0]
    ]
}

# === X de error 8x8 ===
ERROR_X = [
    [1,0,0,0,0,0,0,1],
    [0,1,0,0,0,0,1,0],
    [0,0,1,0,0,1,0,0],
    [0,0,0,1,1,0,0,0],
    [0,0,0,1,1,0,0,0],
    [0,0,1,0,0,1,0,0],
    [0,1,0,0,0,0,1,0],
    [1,0,0,0,0,0,0,1]
]

def show_arrow(name):
    display.fill(0)
    arrow = ARROWS[name]
    for y in range(8):
        for x in range(8):
            display.pixel(x, y, arrow[y][x])
    display.show()

def show_error_x():
    display.fill(0)
    for y in range(8):
        for x in range(8):
            display.pixel(x, y, ERROR_X[y][x])
    display.show()
    time.sleep(0.7)  # duración de la X
    display.fill(0)
    display.show()

# === Juego principal ===
score = 0
options = ["UP", "RIGHT", "LEFT"]

while True:
    arrow = random.choice(options)
    show_arrow(arrow)

    pressed = None

    # Esperar pulsación
    while pressed is None:
        if not btn_sel.value():
            pressed = "UP"
        elif not btn_x.value():
            pressed = "RIGHT"
        elif not btn_y.value():
            pressed = "LEFT"
        time.sleep(0.05)

    # Liberar botón antes de evaluar
    while (not btn_sel.value()) or (not btn_x.value()) or (not btn_y.value()):
        time.sleep(0.05)

    if pressed == arrow:
        score += 1
        display.fill(0)
        display.show()
        time.sleep(0.15)
    else:
        score = 0
        show_error_x()
