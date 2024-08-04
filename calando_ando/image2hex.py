from PIL import Image

def convert_image_to_hex(image_filename):
    # Abrir la imagen
    image = Image.open(image_filename).convert('1')  # Convertir a blanco y negro
  
    # Crear el array de bytes
    hex_array = []
    for y in range(0, 48, 8):
        for x in range(image.width):
            byte = 0
            for bit in range(8):
                if image.getpixel((x, y + bit)) == 0:  # Pixel negro
                    byte |= (1 << bit)
            hex_array.append(byte)

    return hex_array

# Nombre de tu imagen
image_filename = 'C:/TAE_DM/RTOS/UDP_image_LCD/calando_ando/gifs2bmp/pikachu0.bmp'
hex_array = convert_image_to_hex(image_filename)

# Crear la variable con los valores hexadecimales concatenados
hex_string = ''.join(f"{byte:02X}" for byte in hex_array)

# Imprimir el resultado
print("const uint8_t imagenHex[] = {")
for i, byte in enumerate(hex_array):
    if i % 12 == 0:  # Formato para 12 bytes por línea
        print("\n   ", end='')
    print(f"0x{byte:02X}, ", end='')
print("\n};")

# Imprimir la variable con los valores concatenados
print("\nHexadecimal concatenado:", hex_string)
