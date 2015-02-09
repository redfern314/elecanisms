import csv
import matplotlib.pyplot as plt

# NOTE: change this to reflect the sampling period for this data
timescale = 0.0002

with open('processed.csv', 'r') as csvfile:
    reader = csv.reader(csvfile)
    data = [float(point) for point in reader.next()]

    samples = [0]
    for i in range(len(data)-1):
        i += 1
        samples.append(samples[-1]+data[i]*timescale) 

    print samples

    # convert to rps
    data = [1/(point*2*timescale) for point in data]

    plt.plot(samples,data)
    plt.xlabel("Time (s)",fontsize=18)
    plt.ylabel("Motor Speed (revolutions per second)",fontsize=18)
    plt.title("Motor Spin-Down Test",fontsize=24)
    plt.grid(True)
    plt.show()