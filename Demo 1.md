# Step 0 - build everything

```bash
make all
```

# Demo 1 - phtread mutex wrappers

1. show ptmutex-raii.h (fast), then ptmutex-test.cpp

```bash
# different syntax = same behevior
ltrace -e 'pthread*' ./ptmutex-test 1             # lock_guard
ltrace -e 'pthread*' ./ptmutex-test 2             # unique_lock
ltrace -e 'pthread*' ./ptmutex-test 3             # deferred unique_lock + lock_guard
ltrace -e 'pthread*' ./ptmutex-test 4             # unique_lock a deferred unique_lock
ltrace -e 'pthread*' ./ptmutex-test 5             # return lock_guard

# std::mutex optimizations
ltrace -e 'pthread*' ./ptmutex-test 6             # return lock_guard (std::mutex)

# multi-lock
ltrace -e 'pthread*' ./ptmutex-test 7             # std::lock() + adapting lock_guard = C++11
ltrace -e 'pthread*' ./ptmutex-test 8             # std::scoped_lock() = C++17
ltrace -e 'pthread*' ./ptmutex-test 9             # scoped_lock reversed - no lower-address order in libstdc++!
ltrace -e 'pthread*' ./ptmutex-test-clang 9       # scoped_lock reversed - no lower-address order in libc++ either!

# deadlock avoidance
ltrace -f -e 'pthread*' ./ptmutex-test 10         # GCC 11 - retry in order
ltrace -f -e 'pthread*' ./ptmutex-test-gcc12 10   # GCC 12 - retry with round robin (smarter!)
ltrace -f -e 'pthread*' ./ptmutex-test-clang 10   # Clang 19 - round robin too (kudos!)
```

2. show ptmutex-raii.h (error-checking), then ptmutex-test.cpp (cont.)

```bash
# unlock from different thread (can fail!)
ltrace -f -e 'pthread_mutex*' ./ptmutex-test 11   # "error-checking" pthread_mutex - unlock() checks thread
ltrace -f -e 'pthread_mutex*' ./ptmutex-test 12   # std::mutex using "fast" pthread_mutex

# scoped_lock same mutex twice
ltrace -f -e 'pthread_mutex*' ./ptmutex-test 13   # fast pthread_mutex - ignores
```
