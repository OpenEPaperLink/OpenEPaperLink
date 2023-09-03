import sys
from intelhex import IntelHex

def cut_hex_file(input_hex_file, output_bin_file):
    try:
        ih = IntelHex(input_hex_file)

        # Cut the first 16K (16 * 1024 bytes)
        ih = ih[16 * 1024:]

        # Write the remaining data to a binary file
        ih.tobinfile(output_bin_file)

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py input.hex output.bin")
        sys.exit(1)

    input_hex_file = sys.argv[1]
    output_bin_file = sys.argv[2]

    cut_hex_file(input_hex_file, output_bin_file)