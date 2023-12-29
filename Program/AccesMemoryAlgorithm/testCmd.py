import os
import sys
from subprocess import Popen, PIPE, CalledProcessError
import process_interface
from time import sleep
from ctypes import *

#246 1 <-- is in EXE implemented!
#241 2
#235 3
#231 4
secondSearchVal = 241
thirdSearchVal = 235

firstSearch = []
secondSearch = []
thirdSearch = []

def fetchFaders():
    with Popen("memory_scanner.exe", stdout=PIPE, bufsize=1, universal_newlines=True) as p:
        for line in p.stdout:
            firstSearch.append(line.removeprefix("'").removesuffix("\n").strip())
    if p.returncode != 0:
        raise CalledProcessError(p.returncode, p.args)

    print("Finished 1. Search!")
    sleep(5)

    process = process_interface.ProcessInterface()
    process.open("Daslight4")

    for address in firstSearch:
        int_val = c_int.from_buffer(process.read_memory(int(address), buffer_size=8))
        if int_val.value == secondSearchVal:
            secondSearch.append(address)

    print("Finished 2. Search!")
    print(secondSearch)
    sleep(5)

    for address2 in secondSearch:
        int_val2 = c_int.from_buffer(process.read_memory(int(address2), buffer_size=8))
        if int_val2.value == thirdSearchVal:
            thirdSearch.append(address2)



    print("Finished 3. Search!")
    print(thirdSearch)

if __name__ == '__main__':
    try:
        fetchFaders()
    except KeyboardInterrupt:
        print('Interrupted by Keyboard')
        try:
            sys.exit(130)
        except SystemExit:
            os._exit(130)
