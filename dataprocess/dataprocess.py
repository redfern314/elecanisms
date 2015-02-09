import csv

data = []

with open('dump.csv', 'r') as csvfile:
    # get the data from the csv dump
    reader = csv.reader(csvfile)
    data = [int(point) for point in reader.next()]

    # make sure that there are no blips from the readings
    data = [a for a in data if (a > 20 and a < 500)]

with open('processed.csv', 'w') as csvfile:
    # write out modified data back out, overwriting the original data
    writer = csv.writer(csvfile)
    writer.writerow(data)