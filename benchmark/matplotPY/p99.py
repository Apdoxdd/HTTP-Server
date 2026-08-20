import matplotlib.pyplot as plt

c_custom = [100, 500, 1000, 5000, 10000]
p99_custom_ms = [4.8, 27.2, 35.3, 30.5, 794.3]  

c_apache = [100, 500, 1000, 5000]
p99_apache_ms = [7.7, 50.4, 254.6, 4581.1]  # converted to ms

plt.figure(figsize=(10, 6))


plt.plot(
    c_custom,
    p99_custom_ms,
    marker='o',
    color='blue',
    label='My Server',
    linewidth=2,
)
plt.plot(
    c_apache,
    p99_apache_ms,
    marker='s',
    color='tab:orange',
    label='Apache MPM',
    linewidth=2,
)


plt.title(
    'Web Server Benchmark: p99 Latency vs Concurrency\n(1 Million'
    ' Requests/test)'
)
plt.xlabel('Concurrency Level (c)')
plt.ylabel('p99 Latency (ms)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()


plt.tight_layout()
plt.savefig(
    r'c:\Users\EL10_gazy\Pictures\server_p99_benchmark.png',
    dpi=300,
    bbox_inches='tight',
)
plt.show()