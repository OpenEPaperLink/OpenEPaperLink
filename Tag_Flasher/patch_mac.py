import argparse

MZ100_OFFSET = 0x0148
NRF82511_OFFSET = 0x10
ZBS243_OFFSET = 0x10

def patch_mac(file_path, mac, offset):
    try:
        with open(file_path, "r+b") as file:
            mac_bytes = bytearray.fromhex(mac.replace(':', ''))
            while len(mac_bytes) < 8:
                mac_bytes[:0] = bytearray(b'\x00')
            mac_bytes.reverse()  # Reverse the byte order

            file.seek(offset)
            file.write(mac_bytes)
            print(f"MAC address patched successfully.")
    except FileNotFoundError:
        print("Error: File not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

def validate_arguments(args):
    if not (args.nrf82511 or args.zbs243 or args.MZ100):
        print("Either -nrf82511, -zbs243, or MZ100 option is required.")
        return False
    return True

def main():
    parser = argparse.ArgumentParser(description="Patch MAC address in a binary firmware file for the 88MZ100 7.4 inch esl")
    parser.add_argument("-n", "--nrf82511", action="store_true", help="nRF82511 programming")
    parser.add_argument("-z", "--zbs243", action="store_true", help="ZBS243 programming")
    parser.add_argument("-m", "--MZ100", action="store_true", help="88MZ100 7.4 inch programming")
    parser.add_argument("-mac", required=True, help="MAC address")
    parser.add_argument("filename", help="Binary file to patch")

    args = parser.parse_args()


    if not validate_arguments(args):
        return
    
    if args.nrf82511:
        offset = NRF82511_OFFSET
    elif args.zbs243:
        offset = ZBS243_OFFSET
    elif args.MZ100:
        offset = MZ100_OFFSET
    patch_mac(args.filename, args.mac, offset)

if __name__ == "__main__":
    main()
