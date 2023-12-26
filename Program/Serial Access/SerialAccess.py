import serial

ser = serial.Serial()

ser.setPort("COM4")
ser.baudrate = 115200
ser.open()


def checkCommand():
    if(ser.read() == b'#'):
        arduinoID = int(ser.read())
        ser.read()
        targetType = int(ser.read())
        ser.read()
        targetID = int(ser.read())
        ser.read()
        commandType = int(ser.read())
        ser.read()
        commandValue = int(ser.read())
        ser.read()

        command = [arduinoID, targetType, targetID, commandType, commandValue]
        return command

def

while True:
