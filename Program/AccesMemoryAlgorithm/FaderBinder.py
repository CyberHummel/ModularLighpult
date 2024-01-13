import os
import sys
from ctypes import *
from subprocess import Popen, PIPE
from time import sleep

import pygetwindow as gw
import rtmidi
from numpy import interp

import process_interface

faderMidi = [0x90, 21]
faderMemoryAddresses = []
midiout = rtmidi.MidiOut()
ports = midiout.get_ports()
# 246 1 <-- is in EXE implemented!
# 242 2
# 234 3
# 232 4
secondSearchVal = 242
thirdSearchVal = 234
IN_portname = "DaslightIN"
OUT_portname = "DaslightIN"
fader_values = []
mapped_fader_values = []

portnum_IN = 0
portnum_OUT = 0

firstSearch = []
secondSearch = []
thirdSearch = []

def fetchFaders():
    global faderMidi, faderMemoryAddresses, num_faders
    try:
        # Find Daslight 4 window handle
        window_handle = windll.user32.FindWindowW(0, "Daslight 4")
        if window_handle == 0:
            print("Daslight 4 window not found.")
            sys.exit(1)

        # Activate the window by its handle
        windll.user32.ShowWindow(window_handle, 5)  # SW_SHOW
        windll.user32.SetForegroundWindow(window_handle)

    except Exception as e:
        print(f"Error activating Daslight 4 window: {e}")
        sys.exit(1)

    for port in ports:
        if OUT_portname in port:
            portnum_IN = ports.index(port)
    midiout.open_port(portnum_IN)
    for i in range(num_faders):
        midiout.send_message([faderMidi[0], faderMidi[1] + i, 123])

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
    for i in range(num_faders):
        midiout.send_message([faderMidi[0], faderMidi[1] + i, 121])

    process = process_interface.ProcessInterface()
    process.open("Daslight4")

    for address in firstSearch:
        int_val = c_int.from_buffer(process.read_memory(int(address), buffer_size=8))
        if int_val.value == secondSearchVal:
            secondSearch.append(address)

    print("Finished 2. Search!")
    print(secondSearch)
    for i in range(num_faders):
        midiout.send_message([faderMidi[0], faderMidi[1] + i, round(interp(thirdSearchVal, [0, 255], [0, 127]))])
    sleep(0.1)
    for address2 in secondSearch:
        int_val2 = c_int.from_buffer(process.read_memory(int(address2), buffer_size=8))
        if int_val2.value == thirdSearchVal:
            thirdSearch.append(address2)

    print("Finished 3. Search!")
    print(thirdSearch)
    for i in range(num_faders):
        faderMemoryAddresses.append(None)
        if faderMemoryAddresses[i] is None:
            midiout.send_message([faderMidi[0], faderMidi[1] + i, 116])
        else:
            midiout.send_message([faderMidi[0], faderMidi[1] + i, 0])
        sleep(0.1)
        for address3 in thirdSearch:
            int_val3 = c_int.from_buffer(process.read_memory(int(address3), buffer_size=8))
            if int_val3.value == 232:
                faderMemoryAddresses[i] = address3

    for i in range(num_faders):
        midiout.send_message([faderMidi[0], faderMidi[1] + i, 0])
    for i in range(num_faders):
        print("Fader" + str(i) + ":" + str(faderMemoryAddresses[i]))

def send_values_to_midi_channel():
    process = process_interface.ProcessInterface()
    process.open("Daslight4")
    if ports:
        while True:
            for i in range(len(faderMemoryAddresses)):
                if len(fader_values) != len(faderMemoryAddresses):
                    fader_values.append(None)
                fader_values[i] = c_int.from_buffer(process.read_memory(int(faderMemoryAddresses[i]), buffer_size=8)).value
            for i in range(len(fader_values)):
                if len(mapped_fader_values) != len(fader_values):
                    mapped_fader_values.append(None)
                mapped_fader_values[i] = round(interp(fader_values[i], [0, 255], [0, 127]))

            if len(mapped_fader_values) >= 1:
                midiout.send_message([0x90, 18, mapped_fader_values[0]])
            if len(mapped_fader_values) >= 2:
                midiout.send_message([0x90, 19, mapped_fader_values[1]])
            sleep(0.001)
if __name__ == '__main__':
    try:
        num_faders = int(input("Enter the number of faders: "))
        fetchFaders()
        send_values_to_midi_channel()

    except KeyboardInterrupt:
        print('Interrupted by User')
        try:
            sys.exit(130)
        except SystemExit:
            os._exit(130)
