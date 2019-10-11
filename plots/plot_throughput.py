import csv
import matplotlib.pyplot as plt
import sys

if len(sys.argv) < 3:
  print("plot_throughput.py list(<file-name.csv>, <legend-name>) <plot-title>")
  sys.exit()

plt.xlabel('Number of Updates')
plt.ylabel('Throughput')
plt.title(sys.argv[-1])
plt.grid(True)

i = 1
while i < len(sys.argv) - 1:  
  throughputs = []
  update_counts = []

  with open(sys.argv[i]) as f:
    line = f.readline()
    current_time = 0
    current_throughput = 0
    current_update_count = 0

    while line:
      entries = line.strip().split(",")
      current_update_count += 1
      if int(entries[1]) == current_time:
        current_throughput += 1
      else:
        throughputs.append(current_throughput)
        update_counts.append(current_update_count)
        current_throughput = 0
        current_time = int(entries[1])
      line = f.readline()
    # if current_throughput > 0:
    #   throughputs.append(current_throughput)
    #   update_counts.append(current_update_count)
  plt.plot(update_counts, throughputs, label=sys.argv[i + 1])
  i += 2
plt.legend()
plt.show()