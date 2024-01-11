import matplotlib.pyplot as plt

comp_ratio = [
    1.54975, 1.42607, 1.39421, 1.36968,
    1.36769, 1.35397, 1.35089, 1.34422, 1.34433,
    1.33789, 1.34222, 1.33827, 1.33379, 1.33013,
    1.33078, 1.33097, 1.33097, 1.32776, 1.32439,
    1.3198, 1.3198, 1.3153, 1.3153, 1.30897, 1.30897,
    1.30025, 1.30025, 1.30025, 1.30912
]


quantization = range(1, 30)

plt.plot(quantization, comp_ratio, marker='o', linestyle='-', color='b')
plt.title('Compression Ratio vs Quantization')
plt.xlabel('Quantization')
plt.ylabel('Compression Ratio')
plt.grid(True)
plt.show()