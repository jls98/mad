import numpy as np
import matplotlib.pyplot as plt

def parse_file(file_path):
    data = np.loadtxt(file_path)
    data = data[data[:, 1] != 0] #filter 0s
    return data[:, 0], data[:, 1]

def plot_data(x_values, y1_values, y2_values):
    plt.plot(x_values, y1_values, 'green', label='Sqr (green)', linewidth=0.2)
    plt.plot(x_values, y2_values, 'purple', label='Mul (purple)', linewidth=0.2)
    #plt.yscale('log')
    plt.xlim(18000, 18300)
    plt.ylim(30, 2000)
    plt.ylabel('Probe Time')
    plt.legend()
    plt.show()

# Ersetzen Sie 'your_file.txt' durch den tatsächlichen Dateipfad
file_path = 'madgpg_output'

# Datei parsen und Daten extrahieren
y_values_1, y_values_2 = parse_file(file_path)

# Erstellen Sie ein künstliches x-Array basierend auf der Anzahl der Zeilen
x_values_artificial = np.arange(1, len(y_values_1) + 1)

# Daten plotten
plot_data(x_values_artificial, y_values_1, y_values_2)
