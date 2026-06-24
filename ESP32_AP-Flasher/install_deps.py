Import("env")
import subprocess
import sys

def install_python_deps():
    """Install required Python dependencies for esptool"""
    try:
        import intelhex
        print("intelhex module already installed")
    except ImportError:
        print("Installing intelhex module...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "intelhex"])
        print("intelhex module installed successfully")

install_python_deps()
