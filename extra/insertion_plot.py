import matplotlib.pyplot as plt

# Example arrays representing the data
num_points = [10, 100, 500, 1000, 2000, 4000, 8000]
execution_time = [180.327, 1927.76, 24335.9, 80029.7, 205798, 473321, 1.16704e+06]

# Plotting the graph
plt.plot(num_points, execution_time, marker='o')
plt.xlabel('Number of Points')
plt.ylabel('Execution Time (ms)')
plt.title('Execution Time vs Number of Points')
plt.grid(True)
plt.show()
