# HPC-MPI homework3

Timing experiments on server crunchy1 ~ crunchy6:

### Latency of int_ring
single server(crunchy1): 0.000052 comm/sec

two servers(crunchy1,3): 0.000796 comm/sec

five servers(crunchy1-6): 0.000311 comm/sec

### array_ring

single server(crunchy1): Latency: 0.000817 comm/sec

two servers(crunchy1,3): Latency: 0.013701 comm/sec, Bandwidth: 233.554339 MB/s

five servers(crunchy1-6): Latency: 0.005759 comm/sec, Bandwidth: 555.614201 MB/s

