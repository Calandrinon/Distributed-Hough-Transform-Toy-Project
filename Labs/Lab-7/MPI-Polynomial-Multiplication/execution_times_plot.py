import matplotlib.pyplot as plt

polynomial_degrees = [31, 63, 127, 255, 511, 1023, 2047, 4095, 8191, 16383, 32767, 65535]

regular_execution_times =            [0.000123178, 0.000463214, 0.0024296, 0.0133831, 0.0574883, 0.248386, 1.01859, 4.06426, 16.4522, 65.1912, 261.86, 1084.32]
parallel_regular_execution_times =   [0.0013688, 0.00168784, 0.00258903, 0.00154103, 0.00176311, 0.504969, 0.502138, 1.06346, 4.15688, 19.7005, 80.9225, 351.596]
karatsuba_execution_times =          [0.00221326, 0.00746809, 0.0209084, 0.0608691, 0.183356, 0.576981, 1.70614, 5.12932, 15.497, 46.8556, 139.531, 438.625]
parallel_karatsuba_execution_times = [0.00267502, 0.00546831, 0.0264296, 0.0268978, 0.0713695, 0.213727, 0.692647, 1.94257, 5.88652, 22.0241, 58.7718, 188.38]

plt.plot(polynomial_degrees, regular_execution_times, label="Regular multiplication")
plt.plot(polynomial_degrees, parallel_regular_execution_times, label="Parallel regular multiplication")
plt.plot(polynomial_degrees, karatsuba_execution_times, label="Karatsuba multiplication")
plt.plot(polynomial_degrees, parallel_karatsuba_execution_times, label="Parallel Karatsuba multiplication")
plt.xlabel("Polynomial degree")
plt.ylabel("Execution time (seconds)")
plt.legend()
plt.savefig("execution_time_plot.png")

plt.show()

