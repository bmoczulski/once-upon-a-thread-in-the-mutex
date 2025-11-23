#include <iostream>

#ifndef USE_BM_SV
#define USE_BM_SV 1
#endif

#if USE_BM_SV
    #include "BM/synchronized_value.hpp"
    template<typename T>
    using synchronized_value = BM::synchronized_value<T>;
    using BM::apply;
    #define IF_BM_SV(x) x
#else
    #include <experimental/synchronized_value>
    template<typename T>
    using synchronized_value = std::experimental::synchronized_value<T>;
    using std::experimental::apply;
    #define IF_BM_SV(x) /* nothing */
#endif


struct Account 
{
    struct FinancialData
    {
        int balance;
    };
    struct OwnershipData
    {
        std::string owner_name;
        std::string proxy;
    };
    synchronized_value<FinancialData> money;
    synchronized_value<OwnershipData> people;

    // finData HAS been locked at call time, there is no other way :)
    void addAmount(FinancialData &finData, int amount)
    {
        finData.balance += amount;
    }

    void deposit(int amount)
    {
        apply([this, amount](auto &money) {
            addAmount(money, amount);
        }, money);
    }

    void withdraw(int amount)
    {
        deposit(-amount);
    }

    int checkBalance() IF_BM_SV(const)
    {
        // const synchronized_value<T> != synchronized_value<conts T>
        return apply([](const auto &money) {
            return money.balance;
        }, money);
    }

    void transferTo(Account &other, int amount) {
        apply([=](auto &from, auto &to) {
            from.balance -= amount;
            to.balance += amount;
        }, money, other.money);
    }
};

using money_value = int;
struct account {
    money_value balance;
};

#if USE_BM_SV
money_value check_balance(const synchronized_value<account>& account_) {
    return apply([](const auto& account) {
        return account.balance;
    }, account_); // ERROR: incompatible cv-qualifiers in GCC flavor
}
#endif

int main()
{
    Account alice, bob;
    alice.deposit(10);
    alice.transferTo(bob, 2);
    std::cout << "Alice has: $" << alice.checkBalance() << ".00\n";
    std::cout << "Bob has: $" << bob.checkBalance() << ".00\n";
}
