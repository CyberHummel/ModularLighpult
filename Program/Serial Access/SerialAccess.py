import serial

ser = serial.Serial()

ser.setPort("COM4")
ser.baudrate = 115200
ser.open()

def readCommand():
    if(ser.read() == b'#'):
        arduinoID = int(ser.read())
        targetType = int(ser.read())
        targetID = int(ser.read())
        commandType = int(ser.read())
        commandValue = int(ser.readline(3))
        if(ser.read() == b';'):
            command = [arduinoID, targetType, targetID, commandType, commandValue]
            return command
def writeCommand(arduinoID, targetType, targetID, commandType, commandValue):
    ser.write(b'#')
    ser.write(arduinoID)
    ser.write(targetType)
    ser.write(targetID)
    ser.write(commandType)
    if(commandValue < 100):
        ser.write(0)
        if(commandValue < 10):
            ser.write(0)
    ser.write(commandValue)
    ser.write(b';')


while True:
    command = readCommand()
    if(command != None):
        print("Recieved: ")
        print(command)
    writeCommand(0, 0, 0, 0, 100)
