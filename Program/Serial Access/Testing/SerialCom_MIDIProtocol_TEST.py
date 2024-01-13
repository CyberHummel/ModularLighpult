import serial
import time

currentFaderPos = 0
lastFaderPos = 0

arduino = serial.Serial(port='COM9', baudrate=1000000, timeout=.1)


def writeSerial(channel, pitch, velocity, margin):
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
    return data


while True:

    velocity = input("Enter a Velocity: ")

    writeSerial("1", "21", velocity, 1)

    #data = readSerial()
    #if (bytes(";", 'utf-8') in data):
       #print(data)
