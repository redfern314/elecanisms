import csv
import matplotlib.pyplot as plt

# NOTE: change this to reflect the sampling period for this data
timescale = 0.0002

with open('walldata.csv', 'r') as csvfile:
    reader = csv.reader(csvfile)
    data = [int(point) for point in reader.next()]
    newdata = []

    thresh = 200

    for i in range(len(data)):
        bottom = 10
        top = 10
        if (i < 10):
            bottom = i
        if (len(data)-i-1 < 10):
            top = len(data)-i-1
        
        bottomlist = data[i-bottom:i-1]
        toplist = data[i+1:i+top]

        bottomavg = 0
        if len(bottomlist) == 0:
            bottomavg = data[i]
        else:
            bottomavg = sum(bottomlist)/float(len(bottomlist))
        
        topavg = 0
        if len(toplist) == 0:
            topavg = data[i]
        else:
            topavg = sum(toplist)/float(len(toplist))

        print i,data[i],topavg,bottomavg

        if (abs(data[i]-bottomavg) < thresh) or (abs(data[i]-topavg) < thresh):
            newdata.append(data[i])

    data = newdata

    samples = range(len(data))

    plt.plot(samples,data)
    plt.xlabel("Sample Number",fontsize=18)
    plt.ylabel("Joystick Position (degrees)",fontsize=18)
    plt.title("Wall Feedback Response - Filtered",fontsize=24)
    plt.grid(True)
    plt.show()