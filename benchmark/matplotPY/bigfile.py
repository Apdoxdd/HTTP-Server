import matplotlib.pyplot as plt

# Concurrency levels
c_custom = [100, 500, 1000, 5000, 10000]
req_sec_custom = [629.97, 647.44, 675.85, 723.36, 825.20]

c_apache = [100, 500, 1000, 5000, 10000]
req_sec_apache = [2133.65, 2013.22, 2065.85, 249.81, 495.45]

plt.figure(figsize=(10, 6))

# Plotting performance curves
plt.plot(
    c_custom,
    req_sec_custom,
    marker='o',
    color='blue',
    label='My Server',
    linewidth=2,
)
plt.plot(
    c_apache,
    req_sec_apache,
    marker='s',
    color='tab:orange',
    label='Apache MPM',
    linewidth=2,
)

# Graph styling
plt.title(
    'Web Server Benchmark: Throughput vs Concurrency (3.73 MB Large'
    ' File)\n(10,000 Requests/test)'
)
plt.xlabel('Concurrency Level (c)')
plt.ylabel('Requests per Second (req/sec)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()

# Display and save plot
plt.tight_layout()
plt.savefig(
    r'c:\Users\EL10_gazy\Pictures\server_bigfile_benchmark.png',
    dpi=300,
    bbox_inches='tight',
)
plt.show()