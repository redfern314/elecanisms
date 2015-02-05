import serial
import time
import matplotlib.pyplot as plt

data = []

s = serial.Serial('/dev/ttyUSB0', 19200, timeout=None)
time.sleep(.5)
s.flushInput()

ans = raw_input("Enter to collect all waiting lines of data, \"quit\" to exit and display graph:\n")
while ans != "quit":
    collect = s.readlines()

    for datapoint in collect:
        try:
            floatdata = float(datapoint)
            if floatdata > 0 and floatdata < 180:
                data.append(floatdata)
        except Exception, e:
            pass # the data point was invalid; continue anyways

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