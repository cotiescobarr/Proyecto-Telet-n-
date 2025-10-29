# Juego de contar con confirmación
# Raspberry Pi Pico + MAX7219 + 3 botones
# Botones extremos suman +1, botón central confirma

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
btn_x = Pin(14, Pin.IN, Pin.PULL_UP)   # derecha → sumar
btn_sel = Pin(15, Pin.IN, Pin.PULL_UP) # medio → confirmar
btn_y = Pin(16, Pin.IN, Pin.PULL_UP)   # izquierda → sumar

def mostrar_numero(n):
    display.fill(0)
    display.text(str(n), 0, 0)
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

def sumar_si_aprieta(btn):
    if not btn.value():
        time.sleep(0.25)
        while not btn.value(): pass
        return True
    return False

# === Bucle principal del juego ===
while True:
    objetivo = random.randint(1, 9)
    mostrar_numero(objetivo)
    print("Objetivo:", objetivo)

    contador = 0
    confirmado = False

    while not confirmado:
        # Botones que suman
        if sumar_si_aprieta(btn_x) or sumar_si_aprieta(btn_y):
            contador += 1
            print("Contador:", contador)

        # Confirmar con el del medio
        if not btn_sel.value():
            time.sleep(0.15)
            while not btn_sel.value(): pass
            confirmado = True

    # ===== Evaluación =====
    if contador == objetivo:
        print("✅ Correcto")
        mostrar_tick()
    else:
        print("❌ Incorrecto")
        mostrar_x()

    time.sleep(1.5)
    display.fill(0)
    display.show()
 