"""Gzips wwwroot/* into data/www/*.gz.

The output is deterministic: text files are compressed with LF line endings, whatever the checkout
uses (Git on Windows checks them out with CRLF), and the gzip header has no file name or time stamp.
So after a run only the .gz files whose source really changed differ, and only those go in a commit.
"""
import gzip
import os

SOURCE_FOLDER = "wwwroot"
DESTINATION_FOLDER = "data/www"


def normalized(data):
    # a NUL byte means binary (favicon.ico): leave it as it is
    if b"\0" in data:
        return data
    return data.replace(b"\r\r\n", b"\n").replace(b"\r\n", b"\n")


def gzip_files(source_folder, destination_folder):
    os.makedirs(destination_folder, exist_ok=True)
    for file in sorted(os.listdir(source_folder)):
        source_file_path = os.path.join(source_folder, file)
        if not os.path.isfile(source_file_path):
            continue
        destination_file_path = os.path.join(destination_folder, file + ".gz")

        with open(source_file_path, "rb") as f_in:
            compressed = gzip.compress(normalized(f_in.read()), compresslevel=9, mtime=0)

        if os.path.exists(destination_file_path):
            with open(destination_file_path, "rb") as f_old:
                if f_old.read() == compressed:
                    continue
        print(f"Gzipping: {file}")
        with open(destination_file_path, "wb") as f_out:
            f_out.write(compressed)


if __name__ == "__main__":
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    gzip_files(SOURCE_FOLDER, DESTINATION_FOLDER)
