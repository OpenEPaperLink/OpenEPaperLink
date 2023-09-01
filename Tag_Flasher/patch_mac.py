import argparse

def patch_mac(file_path, mac):
    try:
        with open(file_path, "r+b") as file:
            offset = 0x0148
            mac_bytes = bytearray.fromhex(mac)
            mac_bytes.reverse()  # Reverse the byte order

            file.seek(offset)
            file.write(mac_bytes)
            print(f"MAC address patched successfully.")
    except FileNotFoundError:
        print("Error: File not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Patch MAC address in a binary firmware file for the 88MZ100 7.4 inch esl")
    parser.add_argument("-mac", required=True, help="MAC address (without colons)")
    parser.add_argument("filename", help="Binary file to patch")

    args = parser.parse_args()
    patch_mac(args.filename, args.mac)

