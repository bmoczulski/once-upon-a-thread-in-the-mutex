#include <pthread.h>

struct PTMutexBasic {
    pthread_mutex_t m;
    PTMutexBasic()  { pthread_mutex_init(&m, nullptr); }
    ~PTMutexBasic() { pthread_mutex_destroy(&m); }
    // BasicLockable
    void lock()     { pthread_mutex_lock(&m); }
    void unlock()   { pthread_mutex_unlock(&m); }
};

struct PTMutex : public PTMutexBasic {
    // Lockable
    bool try_lock() { return pthread_mutex_trylock(&m) == 0; }
};

































#include <chrono>

// C++14
struct PTMutexTimed : public PTMutexBasic {
    // TimedLockable

    template< class Rep, class Period >
    bool try_lock_for( const std::chrono::duration<Rep, Period>& timeout_duration ) {
        using namespace std::chrono;
        auto ns = duration_cast<nanoseconds>(timeout_duration);

        struct timespec timeout{
            .tv_sec = ns.count() / 1'000'000'000,
            .tv_nsec = ns.count() % 1'000'000'000,
        };
        return pthread_mutex_timedlock(&m, &timeout) == 0;
    }

    template< class Clock, class Duration >
    bool try_lock_until( const std::chrono::time_point<Clock,Duration>& timeout_time ) {
        return try_lock_for(timeout_time - Clock::now());
    }
};





#include <system_error>

struct PTMutexErrorCheckingBasic {
    pthread_mutex_t m;
    PTMutexErrorCheckingBasic() {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&m, &attr);
        pthread_mutexattr_destroy(&attr);
    }
    ~PTMutexErrorCheckingBasic() { pthread_mutex_destroy(&m); }
    // BasicLockable
    void lock() {
        int err = pthread_mutex_lock(&m);
        if (err == EDEADLK) {
            // Same thread trying to lock again - deadlock!
            throw std::system_error(
                std::error_code(
                    static_cast<int>(std::errc::resource_deadlock_would_occur),
                    std::system_category()),
                "Deadlock: thread already owns mutex");
        }
        // Could handle other errors too
    }
    void unlock() {
        int err = pthread_mutex_unlock(&m);
        if (err == EPERM) {
            // Not the owner or not locked
            throw std::system_error(
                std::error_code(
                    static_cast<int>(std::errc::operation_not_permitted),
                    std::system_category()),
                "Unlock failed: not owner or not locked");
        }
    }
};

struct PTMutexErrorChecking : public PTMutexErrorCheckingBasic {
    // Lockable
    bool try_lock() {
        int err = pthread_mutex_trylock(&m);
        if (err == 0) return true;
        if (err == EBUSY) return false;
        if (err == EDEADLK) {
            throw std::system_error(
                std::error_code(
                    static_cast<int>(std::errc::resource_deadlock_would_occur),
                    std::system_category()),
                "try_lock: already own mutex");
        }
        return false;
    }
};
