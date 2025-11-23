#include <mutex>

class Account
{
    std::string owner_name;
    int balance;
    mutable std::mutex m;

public:

    void deposit(int amount) {
        auto l = std::unique_lock(m);
        balance += amount;
    }

    void withdraw(int amount) {
        deposit(-amount);
    }

    int check_balance() {
        return balance;
    }

    void update_owner(const std::string & new_name) {
        owner_name = new_name;
    }
};

int main()
{
    Account a;
    a.deposit(5);
}