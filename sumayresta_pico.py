# Juego de suma y resta con confirmación
# Raspberry Pi Pico + MAX7219 + 3 botones

from machine import Pin, SPI
import time
import random
from max7219 import Matrix8x8

# === Configuración SPI del MAX7219 ===
spi = SPI(0, baudrate=10000000, polarity=0, phase=0,
          sck=Pin(2), mosi=Pin(3))
cs = Pin(5, Pin.OUT)

display = Matrix8x8(spi, cs, 1)
display.brightness(7)
display.fill(0)
display.show()

# === Botones ===
btn_x = Pin(14, Pin.IN, Pin.PULL_UP)   # derecha → +1
btn_sel = Pin(15, Pin.IN, Pin.PULL_UP) # medio → confirmar
btn_y = Pin(16, Pin.IN, Pin.PULL_UP)   # izquierda → -1

# === Patrones mini dígitos 4x5 ===
mini_digitos = {
    '0': ["0110","1001","1001","1001","0110"],
    '1': ["0010","0110","0010","0010","0111"],
    '2': ["0110","1001","0010","0100","1111"],
    '3': ["1110","0001","0110","0001","1110"],
    '4': ["0010","0110","1010","1111","0010"],
    '5': ["1111","1000","1110","0001","1110"],
    '6': ["0111","1000","1110","1001","0110"],
    '7': ["1111","0001","0010","0100","0100"],
    '8': ["0110","1001","0110","1001","0110"],
    '9': ["0110","1001","0111","0001","1110"]
}

# === Funciones de display ===

def mostrar_numero(n):
    display.fill(0)
    s = str(n)
    if len(s) == 1:
        display.text(s, 0, 0)
    else:
        x_offset = -1  # desplazamos 2 columnas hacia la izquierda
        y_offset = 1
        for idx, dig in enumerate(s):
            pattern = mini_digitos[dig]
            for y in range(5):
                for x in range(4):
                    if pattern[y][x] == '1':
                        display.pixel(x + idx*5 + x_offset, y + y_offset, 1)
    display.show()

def mostrar_tick():  # ✔
    tick = [
        "00000000",
        "00000001",
        "00000011",
        "00000110",
        "11001100",
        "01111000",
        "00110000",
        "00000000"
    ]
    display.fill(0)
    for y in range(8):
        for x in range(8):
            if tick[y][x] == "1":
                display.pixel(x, y, 1)
    display.show()

def mostrar_x():  # ❌
    equis = [
        "10000001",
        "01000010",
        "00100100",
        "00011000",
        "00011000",
        "00100100",
        "01000010",
        "10000001"
    ]
    display.fill(0)
    for y in range(8):
        for x in range(8):
            if equis[y][x] == "1":
                display.pixel(x, y, 1)
    display.show()

def mostrar_mas():  # +
    mas = [
        "00011000",
        "00011000",
        "00011000",
        "11111111",
        "11111111",
        "00011000",
        "00011000",
        "00011000"
    ]
    display.fill(0)
    for y in range(8):
        for x in range(8):
            if mas[y][x] == "1":
                display.pixel(x, y, 1)
    display.show()

def mostrar_menos():  # -
    menos = [
        "00000000",
        "00000000",
        "00000000",
        "11111111",
        "11111111",
        "00000000",
        "00000000",
        "00000000"
    ]
    display.fill(0)
    for y in range(8):
        for x in range(8):
            if menos[y][x] == "1":
                display.pixel(x, y, 1)
    display.show()

# === Funciones de botones ===

def ajustar_contador(contador):
    if not btn_x.value():  # derecha → +1
        time.sleep(0.25)
        while not btn_x.value(): pass
        contador += 1
    if not btn_y.value():  # izquierda → -1
        time.sleep(0.25)
        while not btn_y.value(): pass
        contador -= 1
        if contador < 0: contador = 0
    return contador

# === Bucle principal ===
while True:
    # Elegir operación y números
    num1 = random.randint(1, 9)
    op = random.choice(["+", "-"])
    
    if op == "+":
        num2 = random.randint(1, 9)
    else:  # resta
        num2 = random.randint(1, num1)  # para no dar ≤ 0

    objetivo = num1 + num2 if op == "+" else num1 - num2

    # Mostrar primera cifra
    mostrar_numero(num1)
    time.sleep(1)
    
    # Mostrar operación
    if op == "+":
        mostrar_mas()
    else:
        mostrar_menos()
    time.sleep(1)
    
    # Mostrar segunda cifra
    mostrar_numero(num2)
    time.sleep(1)

    # Iniciar contador del jugador
    contador = 0
    confirmado = False
    mostrar_numero(contador)

    while not confirmado:
        contador = ajustar_contador(contador)
        mostrar_numero(contador)
        
        if not btn_sel.value():  # confirmar
            time.sleep(0.15)
            while not btn_sel.value(): pass
            confirmado = True

    # Evaluación
    if contador == objetivo:
        mostrar_tick()
    else:
        mostrar_x()

    time.sleep(1.5)
    display.fill(0)
    display.show()
