import matplotlib.pyplot as plt
import numpy as np

def plot_graph(detections):
    periods = np.arange(1000, len(detections)*1000 + 1000, 1000)

    plt.plot(periods, detections, 'bo', label='Detections')
    plt.xlabel('Periods')
    plt.ylabel('Amount of Detections')
    plt.legend()

    plt.show()

# Beispiel-Array von Detections
detections_array = [168, 316, 459, 604, 748, 876, 924, 947, 941, 952, 945, 951, 945, 940, 954, 946, 940, 940, 958, 948]

# Graph erstellen
plot_graph(detections_array)
