# Once Upon a Thread in the Mutex

[![Presentation Title](slides/Upon%20a%20Thread%20in%20the%20Mutex%2C%20code__dive%202025%2C%2023.10.2025%20%28TITLE%29.png)](slides/Upon%20a%20Thread%20in%20the%20Mutex%2C%20code__dive%202025%2C%2023.10.2025%20%28PDF%29.pdf)

C++ demonstration code exploring thread safety, mutex usage patterns, and the journey to `synchronized_value`.

**▶️ [Watch the talk at code::dive 2025](https://www.youtube.com/watch?v=dgAuPrK-SI8&t=28085s) 🎬**

## Overview

This repository contains demonstration code from the talk "Once Upon a Thread in the Mutex: The Road to `std::synchronized_value<T>` and Why It Matters."

**⚠️ Important**: This code is for educational purposes only. Examples intentionally include bugs and anti-patterns to demonstrate concepts. Do not use in production systems.

## Building and Running

```bash
# Build all targets
make all

# Build specific examples
make ptmutex-test       # pthread RAII wrapper tests
make account-TSA        # Thread Safety Analysis demo
make sv-bm              # synchronized_value demo - the gist of this repo
make avoid              # Deadlock avoidance patterns
```

See the [Makefile](Makefile) for all available targets and compiler requirements.

## Live demos scenarios

- [Demo 1](Demo%201.md) - pthread mutex wrappers
- [Demo 2](Demo%202.md) - Thread Safety Analysis (TSA)
- [Demo 3](Demo%203.md) - synchronized_value

## synchronized_value Example

```cpp
synchronized_value<Account> alice, bob;

// Automatic locking/unlocking
apply([](auto& account) {
    account.balance += 100;
}, alice);

// Multi-value deadlock-free locking
apply([](auto& from, auto& to) {
    from.balance -= 50;
    to.balance += 50;
}, alice, bob);  // No ABBA deadlock possible!
```

## Resources

- **Talk recording**: [code::dive 2025 on YouTube](https://www.youtube.com/watch?v=dgAuPrK-SI8&t=28085s)
- **Slides**: [PDF](slides/Upon%20a%20Thread%20in%20the%20Mutex%2C%20code__dive%202025%2C%2023.10.2025%20%28PDF%29.pdf)
- **My own `synchronized_value` implementation with additions**: [github.com/bmoczulski/synchronized_value](https://github.com/bmoczulski/synchronized_value)

## License

Copyright (c) 2025 Bartosz Moczulski

This work is provided for educational purposes. See [LICENSE.txt](LICENSE.txt) for complete terms.

## Contact

Bartosz Moczulski - moczulski@gmail.com  
https://bartosz.codes/
