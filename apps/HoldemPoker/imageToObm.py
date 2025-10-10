from PIL import Image
import sys

def convert_to_rgb565(r, g, b):
    # Convert RGB888 to RGB565
    r = (r >> 3) & 0x1F
    g = (g >> 2) & 0x3F
    b = (b >> 3) & 0x1F
    return (r << 11) | (g << 5) | b

def png_to_obm_header(png_path, header_path, variable_name):
    # Open image with alpha channel support
    img = Image.open(png_path)
    img = img.convert('RGBA')  # Keep alpha channel
    width, height = img.size
    
    # Convert pixels to RGB565
    pixels = []
    for y in range(height):
        for x in range(width):
            r, g, b, a = img.getpixel((x, y))
            
            # Convert transparent pixels (alpha 0) to bright pink
            if a <= 200:
                r, g, b = 255, 0, 255  # Bright pink (magenta)
            
            pixel = convert_to_rgb565(r, g, b)
            pixels.append(pixel)
    
    # Generate header file
    with open(header_path, 'w') as f:
        f.write('#ifndef __%s_H__\n' % variable_name.upper())
        f.write('#define __%s_H__\n\n' % variable_name.upper())
        f.write('#include <stdint.h>\n\n')
        
        # Write pixel data
        f.write('const uint16_t %s_data[] = {\n' % variable_name)
        for i in range(0, len(pixels), 8):
            line = ['0x%04X' % p for p in pixels[i:i+8]]
            f.write('    ' + ', '.join(line) + ',\n')
        f.write('};\n\n')
        
        # Write OBM header structure
        f.write('const struct OBMHeader %s = {\n' % variable_name)
        f.write('    .signature = 32145,\n')
        f.write('    .width = %d,\n' % width)
        f.write('    .height = %d,\n' % height)
        f.write('    .pixels = %s_data\n' % variable_name)
        f.write('};\n\n')
        
        f.write('#endif // __%s_H__\n' % variable_name.upper())

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print('Usage: python imageToObm.py input.png output.h variable_name')
        sys.exit(1)
        
    png_path = sys.argv[1]
    header_path = sys.argv[2]
    variable_name = sys.argv[3]
    
    png_to_obm_header(png_path, header_path, variable_name)
    print(f'Successfully converted {png_path} to {header_path}')