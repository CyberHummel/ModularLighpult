import serial
import time
import rtmidi

currentFaderPos = 0
lastFaderPos = 0


def writeSerial(channel, pitch, velocity, margin):
    data = arduino.readline().rstrip()
    if not bytes(";", 'utf-8') in data:

        global lastFaderPos
        global currentFaderPos

        currentFaderPos = int(velocity)
        if lastFaderPos - margin > currentFaderPos or lastFaderPos + margin < currentFaderPos:
            arduino.write(bytes(channel, 'utf-8'))
            arduino.write(bytes(":", 'utf-8'))
            arduino.write(bytes(pitch, 'utf-8'))
            arduino.write(bytes(":", 'utf-8'))
            arduino.write(bytes(velocity, 'utf-8'))
            arduino.write(bytes(";", 'utf-8'))
            arduino.flush()
            arduino.reset_input_buffer()
            arduino.reset_output_buffer()
            time.sleep(0.25)

        lastFaderPos = currentFaderPos;

def readSerial():
    data = arduino.readline().rstrip()
    arduino.flush()
    if (bytes(";", 'utf-8') in data):
        data = data.decode('UTF-8')
        data = data.split(':')
        channel = data[0]
        pitch = data[1]
        velocity = data[2].split(";")[0]
        return channel, pitch, velocity


def sendMIDI(data):
    if (data != None):
        midiout = rtmidi.MidiOut()
        midiout.open_port(2)
        with midiout:
            note_on = [0x90, int(data[1]), int(data[2])]
            midiout.send_message(note_on)
        del midiout


arduino = serial.Serial(port='COM3', baudrate=1000000, timeout=.1)

while True:
    #TODO: Implement writing with RAM values and make Loop more Efficient
    writeSerial("1", "24", "100", 1)
    time.sleep(0.002)
    sendMIDI(readSerial())
    writeSerial("1", "24", "0", 1)

