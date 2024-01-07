import subprocess
import sys


def install_package(package):
    subprocess.check_call([sys.executable, "m", "pip", "install", package])


def main():
    try:
        import rtmidi
        import time
        #import weitere Libarys
    except ImportError:
        print('Die erforderliche Bibilothek fehlt bedauerlicherweiße. Die Installation wird gestartet...')
        install_package('rtmid')
        install_package('time')

        import rtmidi
        import time

if __name__ == '__main__':
    main()