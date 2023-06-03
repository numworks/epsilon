import math

def find_angles_on_polar_curve(curve_equation, desired_point, epsilon, angle_range):
    angles = []
    theta = angle_range[0]

    while theta <= angle_range[1]:
        # Calculate point on polar curve at current angle
        point = curve_equation(theta)

        # Compare the calculated point with the desired point
        if distance(point, desired_point) <= epsilon:
            angles.append(theta)

            # Exit the loop if both angles are found
            if len(angles) == 2:
                break

        theta += 0.01  # Increment the angle by a small step size

    return angles

# Example usage
def curve_equation(theta):
    r = math.sin(3 * theta)  # Replace with your actual polar curve equation
    x = r * math.cos(theta)
    y = r * math.sin(theta)
    return (x, y)

desired_point = (0.5, 0.5)  # The point you're looking for on the polar curve
epsilon = 0.01  # Epsilon threshold for point comparison
angle_range = (0, 2 * math.pi)  # Range of angles to search within

result = find_angles_on_polar_curve(curve_equation, desired_point, epsilon, angle_range)
print("Angles:", result)
