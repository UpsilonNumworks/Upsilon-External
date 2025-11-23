from PIL import Image
import sys
import os

def convert_to_rgb565(r, g, b):
    # Convert RGB888 to RGB565
    r = (r >> 3) & 0x1F
    g = (g >> 2) & 0x3F
    b = (b >> 3) & 0x1F
    return (r << 11) | (g << 5) | b

def png_to_header(png_path, header_path, variable_name):
    # Open image with alpha channel support
    img = Image.open(png_path)
    img = img.convert("RGBA") # Keep alpha channel
    width, height = img.size

    # Convert pixels to RGB565
    pixels = []
    for y in range(height):
        for x in range(width):
            r, g, b, a = img.getpixel((x, y))

            # Convert transparent pixels (alpha < 128) to bright pink (magic color for transparency)
            if a < 128:
                r, g, b = 255, 0, 255 # Bright pink (magenta) 0xF81F in RGB565

            pixel = convert_to_rgb565(r, g, b)
            pixels.append(pixel)

    # Generate header file
    with open(header_path, "w") as f:
        f.write(f"#ifndef __{variable_name.upper()}_H__\n")
        f.write(f"#define __{variable_name.upper()}_H__\n\n")
        f.write("#include <stdint.h>\n\n")

        # Write pixel data
        f.write("const uint16_t {variable_name}_data[] = {{\n")
        for i in range(0, len(pixels), 8):
            line = ["0x%04X" % p for p in pixels[i:i+8]]
            f.write("    " + ", ".join(line) + ",\n")
        f.write("};\n\n")

        # Write image info
        f.write(f"#define {variable_name.upper()}_WIDTH {width}\n")
        f.write(f"#define {variable_name.upper()}_HEIGHT {height}\n\n")

        f.write(f"#endif // __{variable_name.upper()}_H__\n")

if __name__ == "__main__":
    # Convert all PNG files in assets directory
    assets_dir = "assets"

    if not os.path.exists(assets_dir):
        print(f"Error: {assets_dir} directory not found")
        sys.exit(1)

    png_files = [f for f in os.listdir(assets_dir) if f.endswith(".png")]

    if not png_files:
        print(f"No PNG files found in {assets_dir}")
        sys.exit(1)

    for png_file in png_files:
        png_path = os.path.join(assets_dir, png_file)
        variable_name = os.path.splitext(png_file)[0].replace("-", "_")
        header_path = os.path.join(assets_dir, f"{variable_name}.h")

        try:
            png_to_header(png_path, header_path, variable_name)
            print(f"Converted {png_file} -> {variable_name}.h")
        except Exception:
            print(f"Error converting {png_file}: {Exception}")

    print(f"\nDone! Converted {len(png_files)} files.")
