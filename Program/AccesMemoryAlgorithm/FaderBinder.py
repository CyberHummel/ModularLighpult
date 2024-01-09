import os
import sys
from ctypes import *
from subprocess import Popen, PIPE
from time import sleep

import pygetwindow as gw
import rtmidi
from numpy import interp
from rtmidi import MidiOut

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
IN_portname = "DaslightIN"
OUT_portname = "Leonardo"

portnum_IN = 0
portnum_OUT = 0

firstSearch = []
secondSearch = []
thirdSearch = []


def fetchFaders():
    global fader1MIDI, fader1MemoryAddress, fader2MemoryAddress, fader2MIDI
    win = gw.getWindowsWithTitle("Daslight 4")[0]
    win.activate()
    for port in ports:
        if IN_portname in port:
            portnum_IN = ports.index(port)
    midiout.open_port(portnum_IN)
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
    midiout.send_message([fader1MIDI[0], fader1MIDI[1], 121])
    midiout.send_message([fader2MIDI[0], fader2MIDI[1], 121])

    process = process_interface.ProcessInterface()
    process.open("Daslight4")

    for address in firstSearch:
        int_val = c_int.from_buffer(process.read_memory(int(address), buffer_size=8))
        if int_val.value == secondSearchVal:
            secondSearch.append(address)

    print("Finished 2. Search!")
    print(secondSearch)
    midiout.send_message([fader1MIDI[0], fader1MIDI[1], round(interp(thirdSearchVal, [0, 255], [0, 127]))])
    midiout.send_message([fader2MIDI[0], fader2MIDI[1], round(interp(thirdSearchVal, [0, 255], [0, 127]))])
    sleep(0.1)
    for address2 in secondSearch:
        int_val2 = c_int.from_buffer(process.read_memory(int(address2), buffer_size=8))
        if int_val2.value == thirdSearchVal:
            thirdSearch.append(address2)

    print("Finished 3. Search!")
    print(thirdSearch)

    midiout.send_message([fader1MIDI[0], fader1MIDI[1], 116])
    midiout.send_message([fader2MIDI[0], fader2MIDI[1], 0])
    sleep(0.1)
    for address3 in thirdSearch:
        int_val3 = c_int.from_buffer(process.read_memory(int(address3), buffer_size=8))
        if int_val3.value == 232:
            fader1MemoryAddress = address3
            break

    midiout.send_message([fader1MIDI[0], fader1MIDI[1], 0])
    midiout.send_message([fader2MIDI[0], fader2MIDI[1], 116])
    sleep(0.1)
    for address4 in thirdSearch:
        int_val4 = c_int.from_buffer(process.read_memory(int(address4), buffer_size=8))
        if int_val4.value == 232:
            fader2MemoryAddress = address4
            break

    midiout.send_message([fader2MIDI[0], fader2MIDI[1], 0])
    print("Fader1: " + str(fader1MemoryAddress))
    print("Fader2: " + str(fader2MemoryAddress))


def send_values_to_midi_channel(fader1_memory_address, fader2_memory_address):
    process = process_interface.ProcessInterface()
    process.open("Daslight4")

    midiout.close_port()

    for port in ports:
        if OUT_portname in port:
            portnum_OUT = ports.index(port)
    midiout.open_port(portnum_OUT)
    if ports:
        while True:

            fader1_value = c_int.from_buffer(process.read_memory(int(fader1_memory_address), buffer_size=8)).value
            fader2_value = c_int.from_buffer(process.read_memory(int(fader2_memory_address), buffer_size=8)).value

            mapped_fader1_value = round(interp(fader1_value, [0, 255], [0, 127]))
            mapped_fader2_value = round(interp(fader2_value, [0, 255], [0, 127]))

            midiout.send_message([0x90,21, mapped_fader1_value])
            midiout.send_message([0x90,22, mapped_fader2_value])

            #TODO Serial implementation of MIDI transfer to Arduino
            sleep(0.001)
if __name__ == '__main__':
    try:
        print(ports)
        fetchFaders()
        send_values_to_midi_channel(fader1MemoryAddress, fader2MemoryAddress)
        midiout.open_port(4)

    except KeyboardInterrupt:
        print('Interrupted by User')
        try:
            sys.exit(130)
        except SystemExit:
            os._exit(130)