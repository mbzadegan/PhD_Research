import random
import math

def estimate_pi(num_points):
    inside_circle = 0

    for _ in range(num_points):
        x = random.uniform(-1, 1)
        y = random.uniform(-1, 1)

        if x**2 + y**2 <= 1:
            inside_circle += 1

    pi_estimate = 4 * inside_circle / num_points
    return pi_estimate

n = 1_000_000
result = estimate_pi(n)

print(f"Estimated pi: {result}")
print(f"Actual pi:    {math.pi}")
print(f"Error:        {abs(result - math.pi)}")
