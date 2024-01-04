import os
import sys
from ctypes import *
from subprocess import Popen, PIPE
from time import sleep

import pygetwindow as gw
import rtmidi
from numpy import interp

import process_interface

fader1MIDI = [0x90, 21]  # 0 = channelNumber1, 1= controllerNum
fader2MIDI = [0x90, 22]
fader1MemoryAddress = 0
fader2MemoryAddress = 0

midiout = rtmidi.MidiOut()
ports = midiout.get_ports()
# 246 1 <-- is in EXE implemented!
# 242 2
# 234 3
# 232 4
secondSearchVal = 242
thirdSearchVal = 234

firstSearch = []
secondSearch = []
thirdSearch = []


def fetchFaders():
    global fader1MIDI, fader1MemoryAddress, fader2MemoryAddress, fader2MIDI
    win = gw.getWindowsWithTitle("Daslight 4")[0]
    win.activate()

    midiout.open_port(1)
    midiout.send_message([fader1MIDI[0], fader1MIDI[1], 123])
    midiout.send_message([fader2MIDI[0], fader2MIDI[1], 123])

    with Popen("memory_scanner.exe", stdout=PIPE, bufsize=1, universal_newlines=True) as p:
        for line in p.stdout:
            firstSearch.append(line.removeprefix("'").removesuffix("\n").strip())
    if p.returncode != 0:
        print("Daslight is not Opened!")
        try:
            sys.exit(130)
        except SystemExit:
            os._exit(130)

    print("Finished 1. Search!")
    sleep(1)
    midiout.send_message([fader1MIDI[0], fader1MIDI[1], 121])
    midiout.send_message([fader2MIDI[0], fader2MIDI[1], 121])


    process = process_interface.ProcessInterface()
    process.open("Daslight4")

    for address in firstSearch:
        int_val = c_int.from_buffer(process.read_memory(int(address), buffer_size=8))
        if int_val.value == secondSearchVal:
            secondSearch.append(address)


    print("Finished 2. Search!")
    sleep(1)
    print(secondSearch)
    midiout.send_message([fader1MIDI[0], fader1MIDI[1], round(interp(thirdSearchVal, [0, 255], [0, 127]))])
    midiout.send_message([fader2MIDI[0], fader2MIDI[1], round(interp(thirdSearchVal, [0, 255], [0, 127]))])


    for address2 in secondSearch:
        int_val2 = c_int.from_buffer(process.read_memory(int(address2), buffer_size=8))
        if int_val2.value == thirdSearchVal:
            thirdSearch.append(address2)

    print("Finished 3. Search!")
    sleep(1)
    print(thirdSearch)

    midiout.send_message([fader1MIDI[0], fader1MIDI[1], 116])
    midiout.send_message([fader2MIDI[0], fader2MIDI[1], 0])
    for address3 in thirdSearch:
        int_val3 = c_int.from_buffer(process.read_memory(int(address3), buffer_size=8))
        if int_val3.value == 232:
            fader1MemoryAddress = address3
            print(address3)
            break

    sleep(0.5)
    midiout.send_message([fader1MIDI[0], fader1MIDI[1], 0])
    midiout.send_message([fader2MIDI[0], fader2MIDI[1], 116])
    for address4 in thirdSearch:
        int_val4 = c_int.from_buffer(process.read_memory(int(address4), buffer_size=8))
        if int_val4.value == 232:
            fader2MemoryAddress = address4
            break

    midiout.send_message([fader2MIDI[0], fader2MIDI[1], 0])
    sleep(0.5)
    print(fader1MemoryAddress)
    print(fader2MemoryAddress)


if __name__ == '__main__':
    try:

        print(ports)
        fetchFaders()

    except KeyboardInterrupt:
        print('Interrupted by User')
        try:
            sys.exit(130)
        except SystemExit:
            os._exit(130)
