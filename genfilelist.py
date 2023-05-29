import os
import json
import hashlib
import sys
import re
from datetime import datetime

def generate_file_hashes(folder_path,internal_path,tag):
    file_list = []
    for file_name in os.listdir(folder_path):
       file_path = os.path.join(folder_path, file_name)
       if os.path.isfile(file_path):
            file_stats = os.stat(file_path)
            md5_hash = calculate_md5(file_path)
            file_info = {
                "name": file_name,
                "path": internal_path + file_name,
                "size": file_stats.st_size,
                "md5": md5_hash,
                "url": "https://raw.githubusercontent.com/"+sys.argv[2]+"/"+sys.argv[3]+"/ESP32_AP-Flasher/data" + internal_path + file_name
            }
            file_list.append(file_info)
    return file_list

def generate_file_hashes2(folder_path,tag):
    file_list = []
    for file_name in os.listdir(folder_path):
       file_path = os.path.join(folder_path, file_name)
       if os.path.isfile(file_path):
            file_stats = os.stat(file_path)
            md5_hash = calculate_md5(file_path)
            file_info = {
                "name": file_name,
                "size": file_stats.st_size,
                "md5": md5_hash,
                "url": "https://github.com/"+sys.argv[2]+"/releases/download/"+tag+"/"+file_name
            }
            file_list.append(file_info)
    return file_list

def calculate_md5(file_path):
    hash_md5 = hashlib.md5()
    with open(file_path, "rb") as file:
        for chunk in iter(lambda: file.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

rp = os.getcwd()

now = datetime.now()
dt_string = now.strftime("%d/%m/%Y %H:%M:%S")
tag = sys.argv[1]

binaries = generate_file_hashes2(rp + "/espbinaries",tag)
tagota = generate_file_hashes2(rp + "/binaries",tag)
files1 = generate_file_hashes(rp + "/ESP32_AP-Flasher/data/www","/www/",tag)
files1.extend(generate_file_hashes(rp + "/ESP32_AP-Flasher/data","/",tag))
files1.extend(generate_file_hashes(rp + "/ESP32_AP-Flasher/data/fonts","/fonts/",tag))

jsonarray = {
    "releaseid": tag,
    "sha": sys.argv[3],
    "repo": sys.argv[2],
    "builddate": dt_string,
    "binaries": binaries,
    "files": files1,
    "tagota": tagota,
    }

json_file_path = "files.json"
with open(json_file_path, "w") as json_file:
    json.dump(jsonarray, json_file, indent=4)
