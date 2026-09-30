Why the Mutex Approach Fails in Low-Latency Trading
OS-Level Locking & Context Switching: std::mutex typically relies on OS primitives (like futex on Linux). If contention occurs, the thread can be put to sleep or forced into a kernel transition, causing latency to spike from nanoseconds to microseconds or even milliseconds—an eternity in high-frequency trading (HFT).

Total Serialization: By locking outside the loop, you have effectively turned multi-threaded code into single-threaded code. t2 cannot do anything until t1 finishes all 1,000,000 increments.

Is std::atomic (fetch_add) the Solution?
Using std::atomic with fetch_add is a massive improvement over a mutex because it avoids OS locks and uses CPU-level instructions (like LOCK XADD on x86 architectures). However, it is still far from ideal if used heavily on a shared variable.

Cache Line Bouncing: If multiple threads are constantly performing atomic operations on the exact same memory address, the CPU cores must constantly invalidate and sync their L1/L2 caches via the MESI protocol. This creates heavy interconnect traffic and slows down the CPU pipeline.

How Low-Latency Systems Actually Handle This
In low-latency systems, developers avoid shared mutable state on the hot path altogether. Here is how you would typically structure something like this:

Thread-Local Storage (TLS): Each thread maintains its own local counter without any synchronization overhead. At the very end of the program (or at the end of the trading day), the main thread aggregates the local results.

C++
// Conceptual pattern
thread_local int local_c = 0;
// ... threads increment their own local_c without locking or atomics ...
Lock-Free Queues (SPSC): If threads actually need to communicate data (e.g., passing orders or market data), they use lock-free Single-Producer Single-Consumer (SPSC) ring buffers with memory ordering carefully tuned (std::memory_order_relaxed or acquire/release).

Actionable Takeaway: Never use mutexes on a trading hot path. While atomics (std::atomic) are safer than mutexes, minimizing shared state using thread-local accumulation is the gold standard for performance.
