import matplotlib.pyplot as plt

# File path to the text file
file_path = "presentation/probs_P1000_B1.txt"

# Read the data from the file
x_points = []
y_points = []

with open(file_path, "r") as file:
    for line in file:
        x, y = map(float, line.split())  # Split the line into x and y values
        x_points.append(x)
        y_points.append(y)

# Plot the data
plt.figure(figsize=(8, 6))
plt.plot(x_points, y_points, marker="o", linestyle="-", color="b", label="Data Points")

# Add title, labels, and legend
plt.title("Placeholder Title")
plt.xlabel("X-Axis Label")
plt.ylabel("Y-Axis Label")
plt.legend()