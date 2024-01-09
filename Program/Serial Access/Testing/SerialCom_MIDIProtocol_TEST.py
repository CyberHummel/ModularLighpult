import serial
import time

arduino = serial.Serial(port='COM9',  baudrate=115200, timeout=.1)

def writeSerial(channel, pitch, velocity):
    arduino.write(bytes(channel,  'utf-8'))
    arduino.write(bytes(":", 'utf-8'))
    arduino.write(bytes(pitch, 'utf-8'))
    arduino.write(bytes(":", 'utf-8'))
    arduino.write(bytes(velocity, 'utf-8'))
    arduino.write(bytes(";", 'utf-8'))
    arduino.flush()

def readSerial():
    data = arduino.readline().rstrip()
    arduino.flush()
    if (bytes(";", 'utf-8') in data):
        return data


while True:
    channel = input("Enter a Channel: ")
    pitch = input("Enter a Pitch: ")
    velocity = input("Enter a Velocity: ")

    writeSerial(channel, pitch, velocity)

    #print(readSerial())
