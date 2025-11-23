#include "ptmutex-raii.h"
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>

template <typename Mutex>
std::lock_guard<Mutex> lock_it(Mutex &m)
{
    return std::lock_guard<Mutex>(m);
}

int main(int argc, char *argv[])
{
    int t = argc > 1 ? atoi(argv[1]) : 0;

    if (1 == t)
    {
        PTMutexBasic m;
        auto lg = std::lock_guard(m);
    }

    if (2 == t)
    {
        PTMutexBasic m;
        auto ul = std::unique_lock(m);
    }

    if (3 == t)
    {
        PTMutexBasic m;
        auto uld = std::unique_lock(m, std::defer_lock);
        auto lg = std::lock_guard(uld);
    }

    if (4 == t)
    {
        PTMutexBasic m;
        auto uld = std::unique_lock(m, std::defer_lock);
        // auto ulul = std::unique_lock(uld);  // ERROR: copy constructor = delete
        auto ulul = std::unique_lock<std::unique_lock<PTMutexBasic>>(uld);
    }

    if (5 == t)
    {
        PTMutexBasic m;
        auto l = lock_it(m);
    }

    if (6 == t)
    {
        std::mutex m;
        auto l = lock_it(m);
    }

    if (7 == t)
    {
        PTMutex m1, m2;
        std::lock(m1, m2); // Meh, so C++11!
        auto l1 = std::lock_guard(m1, std::adopt_lock);
        auto l2 = std::lock_guard(m2, std::adopt_lock);
    }

    if (8 == t)
    {
        PTMutex m1, m2;
        auto l = std::scoped_lock(m1, m2); // C++17
    }

    if (9 == t)
    {
        PTMutex m1, m2;
        { auto l = std::scoped_lock(m1, m2); }
        { auto l = std::scoped_lock(m2, m1); }
    }

    if (10 == t)
    {
        PTMutex m1, m2, m3;
        using namespace std::chrono_literals;
        std::atomic<bool> m3_is_locked = false;

        std::jthread j([&] {
            std::lock_guard l(m3);
            m3_is_locked = true;
            std::this_thread::sleep_for(100ms);
        });

        while (!m3_is_locked) {} // spin-wait
        auto l = std::scoped_lock(m1, m2, m3);
    }

    if (11 == t)
    {
        PTMutexErrorCheckingBasic m;
        std::jthread j([l = std::unique_lock(m)] {
            // cross-thread unlock = ERROR
        });
    }

    if (12 == t)
    {
        std::mutex m;
        std::jthread j([l = std::unique_lock(m)] {
            // cross-thread unlock = ERROR
        });
    }

    if (13 == t)
    {
        PTMutex m;
        auto l = std::scoped_lock(m, m); // oops! m repeaded :(
    }
}

