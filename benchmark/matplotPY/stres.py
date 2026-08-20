import matplotlib.pyplot as plt

c_custom = [100, 500, 1000, 5000, 10000, 15000, 20000, 25000]
req_sec_custom = [74663.79, 70217.55, 70660.96, 67906.72, 62632.71, 43709.88, 48442.15, 29257.13]

c_apache = [100, 500, 1000, 5000]
req_sec_apache = [55368.50, 34350.13, 33517.64, 3575.69]

plt.figure(figsize=(10, 6))

plt.plot(c_custom, req_sec_custom, marker='o', color='blue', label='My Server', linewidth=2)
plt.plot(c_apache, req_sec_apache, marker='s', color='tab:orange', label='Apache MPM', linewidth=2)

plt.title('Web Server Benchmark: Throughput vs Concurrency\n' \
'  (1 Million Requests/test)')
plt.xlabel('Concurrency Level (c)')
plt.ylabel('Requests per Second (req/sec)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()

#  plot
plt.tight_layout()
plt.savefig(
    r'c:\Users\EL10_gazy\Pictures\server_benchmark.png',
    dpi=300,
    bbox_inches='tight',
)
plt.show()