import subprocess
from subprocess import Popen, PIPE

process = Popen(["main.exe"], stdout=subprocess.PIPE)
while True:
    print(process.stdout.readline())