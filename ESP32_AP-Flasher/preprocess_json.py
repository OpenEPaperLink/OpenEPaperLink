#!/usr/bin/python3

import os
import subprocess
import glob

# create runtime wwwroot/*.json from src/*.json
source_folder = "src"  # Replace with the path of the source folder
destination_folder = "wwwroot"  # Replace with the path of the destination folder

# Create the destination folder if it doesn't exist
if not os.path.exists(destination_folder):
    os.makedirs(destination_folder)

# Get a list of all files in the source folder
files = glob.glob(f'{source_folder}/*.json')

for source_file_path in files:
    file = os.path.basename(source_file_path)
    destination_file_path = os.path.join(destination_folder, file)
    
    print(f"preprocessing: {file}")
    cmd_line = [ 'cpp', '-P',f'{source_file_path}','-o' f'{destination_file_path}' ]
    if 'Import' in globals() and callable(Import):
        Import("env")
        for define in env.get('BUILD_FLAGS', []):
            if define.startswith('-D CONTENT'):
                cmd_line.append(define)
    print(f'Running {cmd_line}')

    subprocess.run(cmd_line)

