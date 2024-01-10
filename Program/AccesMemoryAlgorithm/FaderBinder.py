import os
import sys
from ctypes import *
from subprocess import Popen, PIPE
from time import sleep

import pygetwindow as gw
import rtmidi
from numpy import interp

import process_interface

import serial


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
OUT_portname = "DaslightOUT"

portnum_IN = 0
portnum_OUT = 0

firstSearch = []
secondSearch = []
thirdSearch = []

mapped_fader1_valueOld = 0
mapped_fader1_valueOld = 0

arduino = serial.Serial(port='COM9', baudrate=115200, timeout=.1)

sending = False

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


def writeSerial(channel, pitch, velocity):
    arduino.write(bytes(str(channel), 'utf-8'))
    arduino.write(bytes(":", 'utf-8'))
    arduino.write(bytes(str(pitch), 'utf-8'))
    arduino.write(bytes(":", 'utf-8'))
    arduino.write(bytes(str(velocity), 'utf-8'))
    arduino.write(bytes(";", 'utf-8'))
    arduino.flush()
    arduino.reset_output_buffer()


def readSerial():
    data = arduino.readline().rstrip()
    # arduino.flush()
    arduino.reset_input_buffer()
    return data


def send_values_to_midi_channel(fader1_memory_address, fader2_memory_address, process):
    global mapped_fader1_valueOld
    global mapped_fader2_valueOld

    data = readSerial()
    if not bytes(";", 'utf-8') in data:
        if arduino.readline().rstrip() != None:
            fader1_value = c_int.from_buffer(process.read_memory(int(fader1_memory_address), buffer_size=8)).value
            fader2_value = c_int.from_buffer(process.read_memory(int(fader2_memory_address), buffer_size=8)).value

            mapped_fader1_value = round(interp(fader1_value, [0, 255], [0, 127]))
            mapped_fader2_value = round(interp(fader2_value, [0, 255], [0, 127]))

            if (mapped_fader1_value != mapped_fader1_valueOld):
                writeSerial(1, 21, mapped_fader1_value)
                mapped_fader1_valueOld = mapped_fader1_value
                print("sad")
                sleep(0.05)



        sleep(0.005)


def read_values_to_midi_channel():
    data = readSerial()
    if (data != None and bytes(";", 'utf-8') in data):
        data = str(data)
        data = data.split(":")
        if len(data) == 3:
            channel = data[0].removeprefix("b'")
            pitch = data[1]
            value = data[2].removesuffix(";'")

            if (channel == "1"):
                if (pitch == "21"):
                    print(value)
                    midiout.send_message([fader1MIDI[0], fader1MIDI[1], int(value)])


if __name__ == '__main__':
    try:
        print(ports)
        fetchFaders()

        process = process_interface.ProcessInterface()
        process.open("Daslight4")


        #sending = Thread(target=send_values_to_midi_channel, args=(fader1MemoryAddress, fader2MemoryAddress, process))
        #receiving = Thread(target=read_values_to_midi_channel)

        #sending.start()
        #receiving.start()
        while True:
            read_values_to_midi_channel()
            sleep(0.01)
            send_values_to_midi_channel(fader1MemoryAddress, fader2MemoryAddress, process)
        #TODO better Responsiveness
        #TODO sendingMargins



    except KeyboardInterrupt:
        print('Interrupted by User')
        try:
            sys.exit(130)
        except SystemExit:
            os._exit(130)
