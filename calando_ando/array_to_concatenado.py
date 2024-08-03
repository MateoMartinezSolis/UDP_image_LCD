# Lista de valores hexadecimales
hex_values = [
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0,
   0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xfc, 0x3f, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xe0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd0, 0xff,
   0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0xd8, 0xff, 0xff, 0x03, 0x00, 0x00,
   0x00, 0x00, 0xf8, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff,
   0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x1f, 0x00, 0x00,
   0x00, 0x00, 0xf8, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff,
   0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x3f, 0x00, 0x00,
   0x00, 0x00, 0xfc, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff,
   0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0x3f, 0x00, 0x00,
   0x00, 0x00, 0xfc, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff,
   0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0x3f, 0x00, 0x00,
   0x00, 0x00, 0xfc, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff,
   0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x3f, 0x00, 0x00,
   0x00, 0x00, 0xf8, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff,
   0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x0f, 0x00, 0x00,
   0x00, 0x00, 0xe0, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff,
   0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc,
   0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
]

# Contar los valores en el arreglo
count = len(hex_values)

print(f"El arreglo contiene {count} valores.")

# Convertir los valores hexadecimales a una cadena concatenada
hex_string = ''.join(f'{value:02x}' for value in hex_values)

print(hex_string)