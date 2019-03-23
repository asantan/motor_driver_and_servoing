import serial 
import sys
import time

ser = serial.Serial(
        port = '/dev/tty1',  
        baudrate = 9600, 
        parity = serial.PARITY_NONE, 
        stopbits = serial.STOPBITS_ONE, 
        bytesize = serial.EIGHTBITS, 
        timeout = 1)

print("Connected to: " + ser.portstr)

print("Number of arguments " + str(len(sys.argv)))

while True:
    cli_arg = input("Input a number from -360 to 360\n")
    if cli_arg.isdigit():
        cli_arg =[ int(cli_arg) ]
        ser.write(cli_arg)
        print("Value written: " + str(cli_arg))
    else:
        print("Please input a number")

