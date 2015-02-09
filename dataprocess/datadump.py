import serial
import time
import csv
import matplotlib.pyplot as plt

data = []

s = serial.Serial('/dev/ttyUSB1', 19200, timeout=0)
time.sleep(.5)
s.flushInput()

ans = raw_input("Enter nothing to collect all waiting lines of data, \"graph\" to display graph, \"save\" to write all data to file, or \"exit\" to exit.\n")
while ans != "exit":
    if ans == "graph":
        samples = range(len(data))

        plt.plot(samples,data)
        plt.xlabel("Sample number")
        plt.ylabel("Reading")
        plt.title("Serial Data Readings")
        plt.grid(True)
        plt.show()

    elif ans == "save":
        with open('dump.csv', 'wb') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(data)

    elif ans == "":
        collect = s.readlines()
        
        for datapoint in collect:
            try:
                # data.append(int(datapoint))
                floatdata = float(datapoint)
                if floatdata > 0 and floatdata < 180:
                    data.append(floatdata)
            except Exception, e:
                pass # the data point was invalid; continue anyways

        print data

    ans = raw_input("Enter nothing to collect all waiting lines of data, \"graph\" to display graph, \"save\" to write data to file, or \"exit\" to exit.\n")

s.close()

    