import serial
import matplotlib.pyplot as plt

data = []

s = serial.Serial('/dev/ttyUSB0', 9600, timeout=0)

ans = input("Enter to collect up to 100 bytes of data, \"quit\" to exit and display graph:")
while ans != "quit":
    collect = s.read(100)
    print collect
    # TODO: append collect to data and convert to proper types
    ans = input("Enter to collect up to 100 bytes of data, \"quit\" to exit and display graph:")

s.close()

samples = range(len(data))

plt.plot(samples,data)
plt.xlabel("Sample number")
plt.ylabel("Reading")
plt.title("Serial Data Readings")
plt.grid(True)
plt.show()