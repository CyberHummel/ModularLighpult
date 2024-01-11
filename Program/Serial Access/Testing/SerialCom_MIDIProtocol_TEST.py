import serial
import time

arduino = serial.Serial(port='COM9',  baudrate=1000000, timeout=.1)

def writeSerial(channel, pitch, velocity):
    arduino.write(bytes(channel,  'utf-8'))
    arduino.write(bytes(":", 'utf-8'))
    arduino.write(bytes(pitch, 'utf-8'))
    arduino.write(bytes(":", 'utf-8'))
    arduino.write(bytes(velocity, 'utf-8'))
    arduino.write(bytes(";", 'utf-8'))
    arduino.flush()
    arduino.reset_input_buffer()
    arduino.reset_output_buffer()

def readSerial():
    data = arduino.readline().rstrip()
    arduino.flush()
    return data


while True:

    velocity = input("Enter a Velocity: ")

    writeSerial("1", "21", velocity)
    data = readSerial()
    if(bytes(";", 'utf-8') in data):
        print(data)
