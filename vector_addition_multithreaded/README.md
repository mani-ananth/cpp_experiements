# Multithreaded Vector summation benchmarking
This is an experiment I designed to understand `std::thread` usage and also get accustomed to some multithreaded benchmarking using `google-benchmark`.
End outcome is quite simple: summation of all elements in a large vector (100s of millions).
I have tried to parallelize it using `std::thread` and also benchmark in ST and MT modes using `google-benchmark`

## Results
1. One important thing while benchmarking is disabling CPU Frequency scaling. You'll see some inconsistent results without it, but `google-benchmark` will warn about it.

`sudo cpupower frequency-set --governor performance`

2. These are the results for summing up 100 million integers across many threads. The library reports 2 numbers:
   - CPU: Average CPU time per iteration of the benchmark run
   - Time: Average wall time per iteration

3. CPU: AMD Ryzen 3900X 12 core

4. The 1 thread case has 2 measurements using the Single-Thread (`mode=1`) and Multi-Threaded (`mode=2`) Benchmarks

Update: seeing some inconsistent results with thread enabled. Will add results here once I figure out the issue
