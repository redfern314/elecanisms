import serial
import time
import matplotlib.pyplot as plt

data = []

s = serial.Serial('/dev/ttyUSB0', 19200, timeout=0)
time.sleep(.5)
s.flushInput()

ans = raw_input("Enter to collect all waiting lines of data, \"quit\" to exit and display graph:\n")
while ans != "quit":
    collect = s.readlines()
    print collect
    floatarray = [float(a) for a in collect]
    data += floatarray
    print data

    ans = raw_input("Enter to collect all waiting lines of data, \"quit\" to exit and display graph:\n")

s.close()

samples = range(len(data))

plt.plot(samples,data)
plt.xlabel("Sample number")
plt.ylabel("Reading")
plt.title("Serial Data Readings")
plt.grid(True)
plt.show()