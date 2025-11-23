#include <mutex>
#include <thread>
#include <experimental/synchronized_value>

template<typename T>
using synchronized_value = std::experimental::synchronized_value<T>;
using std::experimental::apply;

void unlock_in_another_thread()
{
    std::mutex m;
    std::jthread([l = std::unique_lock(m)]() {
        // nothing
    });
}

template<typename T, typename M = std::mutex>
class ProxyLockable
{
    T data;
    mutable M m;

public:
    class Proxy
    {
        T& data;
        std::unique_lock<M> l;
    public:
        Proxy(T &data, std::unique_lock<M>l)
            : data(data)
            , l(std::move(l))
        {}

        T& operator*() { return data; }
        T* operator->() { return &data; }
    };

    auto lock() {
        return Proxy(data, std::unique_lock(m));
    }
};

struct S { int a, b; };

void abba_sends_their_regards()
{
    ProxyLockable<S> A;
    ProxyLockable<S> B;

    auto a = A.lock();
    auto b = B.lock();

    a->a = 42;
    *b = {666, 666};
}

void abba_sends_less_regards()
{
    synchronized_value<int> sv_a;
    synchronized_value<int> sv_b;
    apply([&](int &a) {
        apply([&](int &b) {
            a = 42;
            b = 666;
        }, sv_b);
    }, sv_a);
}

void limited_abba()
{
    synchronized_value<int> sv_a;
    synchronized_value<int> sv_b;
    apply([](int &a, int &b) {
        a = 42;
        b = 666;
    }, sv_a, sv_b);
}

int main()
{
    unlock_in_another_thread();
    abba_sends_their_regards();
    abba_sends_less_regards();
    limited_abba();
}
