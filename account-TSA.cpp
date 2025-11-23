#include "tsa.h"
#include <mutex>
#include <thread>
#include <iostream>

class Account
{
    std::string owner_name  GUARDED_BY(M);
    int balance             GUARDED_BY(m) = 0;
    mutable std::mutex m    ACQUIRED_BEFORE(M);
    mutable std::mutex M    ACQUIRED_AFTER(m);

public:
    int bonus_locked(int amount) REQUIRES(m) {
        if (balance >= 1'000'000 && amount >= 1'000) {
            return amount / 100;
        }
        return 0;
    }

    int bonus(int amount) {
        std::lock_guard<std::mutex> g(m);
        return bonus_locked(amount);
    }

    void deposit(int amount) {
        std::lock_guard<std::mutex> g(m);
        // std::unique_lock<std::mutex> l(m);
        deposit_locked(amount);
    }

private:
    void deposit_locked(int amount) REQUIRES(m) {
        balance += amount + bonus_locked(amount);
    }

public:
    void withdraw(int amount) {
        deposit(-amount);
    }

    int check_balance() {
        std::lock_guard<std::mutex> g(m);
        return balance;
    }

    void update_owner(const std::string & new_name) {
        std::lock_guard<std::mutex> G(M);
        update_owner_locked(new_name);
    }

    std::string get_owner() {
        // std::unique_lock<std::mutex> G(M);
        // if (rand() % 2 == 0) {
        //     G.unlock();
        // }
        std::lock_guard<std::mutex> G(M);
        return owner_name;
    }

private:
    void update_owner_locked(const std::string & new_name) REQUIRES(M){
        owner_name = new_name;
    }

public:
    void deposit_with_owner_update(int amount, const std::string & new_name) {
        std::lock_guard<std::mutex> g(m);
        std::lock_guard<std::mutex> G(M);
        update_owner_locked(new_name);
        deposit_locked(amount);
    }

    // void deposit_with_owner_update_ABBA(int amount, const std::string & new_name) {
    //     std::lock_guard<std::mutex> G(M); // Requires extra flag: -Wthread-safety-beta
    //     std::lock_guard<std::mutex> g(m); // See: https://github.com/llvm/llvm-project/issues/51788
    //     update_owner_locked(new_name);
    //     deposit_locked(amount);
    // }
};

int main()
{
    Account a;
    a.deposit(1'000'000);
    std::cout << "A bonus for depositing 1000 would be: " << a.bonus(1000) << "\n";
    a.deposit_with_owner_update(1'000, "John Doe");
    // a.deposit_with_owner_update_ABBA(1'000, "John Doe");
    std::cout << a.get_owner() << " has: $" << a.check_balance() << ".00\n";
}

