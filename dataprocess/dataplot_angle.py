import csv
import matplotlib.pyplot as plt

# NOTE: change this to reflect the sampling period for this data
timescale = 0.0002

with open('anglecalraw.csv', 'r') as csvfile:
    reader = csv.reader(csvfile)
    data = [float(point) for point in reader.next()]

    samples = range(len(data))

    plt.plot(samples,data)
    plt.xlabel("Time (s)")
    plt.ylabel("Motor Angular Speed (revolutions per second)")
    plt.title("Angle Calibration Test")
    plt.grid(True)
    plt.show()