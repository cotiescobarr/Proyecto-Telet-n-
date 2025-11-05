from machine import Pin, SPI
import time
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
btn_x = Pin(14, Pin.IN, Pin.PULL_UP)   # derecha → subir modo
btn_sel = Pin(15, Pin.IN, Pin.PULL_UP) # medio → confirmar
btn_y = Pin(16, Pin.IN, Pin.PULL_UP)   # izquierda → bajar modo

# === Funciones básicas ===

def mostrar_numero(n):
    display.fill(0)
    display.text(str(n), 0, 0)
    display.show()

def boton_presionado(btn):
    if not btn.value():
        time.sleep(0.25)
        while not btn.value():
            pass
        return True
    return False

def botones_extremos_3s():
    if not btn_x.value() and not btn_y.value():
        inicio = time.ticks_ms()
        while not btn_x.value() and not btn_y.value():
            if time.ticks_diff(time.ticks_ms(), inicio) > 3000:  # 3 segundos
                return True
            time.sleep(0.05)
    return False

# === Función del menú principal ===
def menu_principal():
    modo = 1
    juego_seleccionado = 0
    mostrar_numero(modo)
    while juego_seleccionado == 0:
        if boton_presionado(btn_x):
            modo += 1
            if modo > 5:
                modo = 1
            mostrar_numero(modo)
        if boton_presionado(btn_y):
            modo -= 1
            if modo < 1:
                modo = 5
            mostrar_numero(modo)
        if boton_presionado(btn_sel):
            juego_seleccionado = modo
            display.fill(0)
            display.text("OK", 0, 0)
            display.show()
            time.sleep(0.5)
            display.fill(0)
            display.show()
    return juego_seleccionado


def ejecutar_juego(juego):
    try:
        if hasattr(juego, "main"):
            juego.main()  # ejecuta el juego una vez
    except Exception as e:
        print(f"Error en el juego: {e}")
        display.fill(0)
        display.text("Error", 0, 0)
        display.show()
        time.sleep(2)
    finally:
        # Limpiar display y asegurar que volvemos al menú
        display.fill(0)
        display.show()
        # Pequeña pausa para evitar rebotes
        time.sleep(0.5)

# === Bucle principal ===
import sys

# === Bucle principal ===
while True:
    seleccion = menu_principal()

    # Borrar display antes de cargar el juego
    display.fill(0)
    display.show()

    # Determinar nombre del juego según selección
    if seleccion == 1:
        nombre_juego = "contadordenumeros_pico"
    elif seleccion == 2:
        nombre_juego = "juegodirecciones_pico"
    elif seleccion == 3:
        nombre_juego = "finalmemory_pico"
    elif seleccion == 4:
        nombre_juego = "tateti_pico"
    elif seleccion == 5:
        nombre_juego = "sumayresta_pico"
    else:
        continue

    # Eliminar módulo previo (si está en memoria)
    if nombre_juego in sys.modules:
        del sys.modules[nombre_juego]

    # Importar de forma dinámica
    juego = __import__(nombre_juego)

    # Ejecutar el juego dentro del wrapper
    ejecutar_juego(juego)

    # Asegurar que la pantalla quede limpia al volver
    display.fill(0)
    display.show()
    time.sleep(0.3)
#version final de la interfaz
