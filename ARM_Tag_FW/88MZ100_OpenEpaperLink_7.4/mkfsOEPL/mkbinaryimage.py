import sys
from PIL import Image
import struct
import math

def convert_image_to_binary(input_image_path, output_file_path):
    # Open the image
    img = Image.open(input_image_path)

    # Calculate the padded X size
    new_x_size = math.ceil(img.size[0] / 8) * 8

    # Create a new image with the padded X size
    img_padded = Image.new("RGB", (new_x_size, img.size[1]), (255, 255, 255))
    img_padded.paste(img, (0, 0))

    # Convert the padded image to grayscale and invert
    img_bw = img_padded.convert('L').point(lambda x: 255 - x)

    # Check if the image has a red channel
    has_red_channel = 'R' in img.getbands()
    pixel_data_red = img_padded.split()[0].point(lambda x: 255 - x).tobytes()
    if any(pixel_data_red):
        has_red_channel = 1;
    else:
        has_red_channel = 0;

    # Calculate unpadded resolution
    unpadded_resolution = (img.size[0], img.size[1])

    # Create binary file
    with open(output_file_path, "wb") as binary_file:
        # Write header: 0x06, unpadded X size, Y size, bits-per-pixel
        binary_file.write(b'\x06')
        binary_file.write(unpadded_resolution[0].to_bytes(2, byteorder='little'))
        binary_file.write(unpadded_resolution[1].to_bytes(2, byteorder='little'))

        # Determine bits-per-pixel
        bits_per_pixel = 2 if has_red_channel else 1
        binary_file.write(bits_per_pixel.to_bytes(1, byteorder='big'))

        # Extract pixel data
        pixel_data_bw = img_bw.tobytes()

        # If there's a red channel, extract red pixels
        if has_red_channel:
            pixel_data_red = img_padded.split()[0].point(lambda x: 255 - x).tobytes()

            # Process pixel data in chunks of 8 and pack into bytes for black/white
            packed_data_bw = bytearray()
            packed_data_red = bytearray()
            for i in range(0, len(pixel_data_bw), 8):
                chunk_bw = pixel_data_bw[i:i + 8]
                packed_byte_bw = 0
                for j, pixel_value in enumerate(chunk_bw):
                    packed_byte_bw |= (pixel_value >> j) & 1 << (7 - j)

                packed_data_bw.append(packed_byte_bw)

                chunk_red = pixel_data_red[i:i + 8]
                packed_byte_red = 0
                for j, pixel_value in enumerate(chunk_red):
                    packed_byte_red |= (pixel_value >> j) & 1 << (7 - j)

                packed_byte_red = packed_byte_red ^ packed_byte_bw

                packed_data_red.append(packed_byte_red)


            # Write packed pixel data to binary file
            binary_file.write(bytes(packed_data_bw))
            binary_file.write(bytes(packed_data_red))
        else:
            # Process pixel data in chunks of 8 and pack into bytes for black/white
            packed_data_bw = bytearray()
            for i in range(0, len(pixel_data_bw), 8):
                chunk_bw = pixel_data_bw[i:i + 8]
                packed_byte_bw = 0
                for j, pixel_value in enumerate(chunk_bw):
                    packed_byte_bw |= (pixel_value >> j) & 1 << (7 - j)

                packed_data_bw.append(packed_byte_bw)

            # Write packed pixel data to binary file
            binary_file.write(bytes(packed_data_bw))

    print(f"Conversion completed. Output saved to {output_file_path}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script_name.py input_image_path output_file_path")
    else:
        input_image_path = sys.argv[1]
        output_file_path = sys.argv[2]
        convert_image_to_binary(input_image_path, output_file_path)
