from subprocess import Popen, PIPE, CalledProcessError
import process_interface
from time import sleep
import ctypes

#246 1 <-- is in EXE implemented!
#241 2
#235 3
#231 4
secondSearchVal = 241

firstSearch = []
secondSearch = []

with Popen("memory_scanner.exe", stdout=PIPE, bufsize=1, universal_newlines=True) as p:
    for line in p.stdout:
        firstSearch.append(line.removeprefix("'").removesuffix("\n").strip())
if p.returncode != 0:
    raise CalledProcessError(p.returncode, p.args)

print("Finished 1. Search!")
sleep(10)

Process = process_interface.ProcessInterface()
Process.open("Daslight4")

for address in firstSearch:
    intvalue = ctypes.c_int.from_buffer(Process.read_memory(address, 4))
    if intvalue == secondSearchVal:
        secondSearch.append(address)

print("Finished 2. Search!")
print(secondSearch)
