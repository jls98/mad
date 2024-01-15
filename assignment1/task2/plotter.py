import matplotlib.pyplot as plt
from collections import Counter

# Sample array (replace with your own data)
data_cached = [218, 218, 220, 224, 236, 238, 240, 240, 240, 240, 240, 240, 240, 240, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 248, 248, 248, 248, 250, 256, 276, 282, 284]#data_uncached = [236, 236, 236, 236, 236, 236, 236, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 244, 244, 244, 244, 244, 244, 246, 246, 252, 646]

#= [28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32]
## Count the frequency of each value in the array
value_counts_cached = Counter(data_cached)
#value_counts_uncached = Counter(data_uncached)

# Extract unique values and their frequencies
values_cached = list(value_counts_cached.keys())
#values_uncached = list(value_counts_uncached.keys())
frequencies_cached = list(value_counts_cached.values())
#frequencies_uncached = list(value_counts_uncached.values())

# Create a bar plot
plt.bar(values_cached, frequencies_cached, color='red')
#plt.bar(values_uncached, frequencies_uncached, color='red')

# Label the axes
plt.xlabel("Values")
plt.ylabel("Frequency")

# Display the plot
plt.show()