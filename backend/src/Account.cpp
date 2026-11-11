#include "../include/Account.h"
#include <stdexcept>
Account::Account(int id, std::string holder, double balance)
 : id_(id), holder_(std::move(holder)), balance_(balance) {}
int Account::id() const { return id_; }
const std::string& Account::holder() const { return holder_; }
double Account::balance() const { return balance_; }
void Account::deposit(double amount) {
 if (amount <= 0) throw std::runtime_error("Amount must be greater than zero");
 balance_ += amount;
}
bool Account::withdraw(double amount) {
 if (amount <= 0) throw std::runtime_error("Amount must be greater than zero");
 if (amount > balance_) return false;
 balance_ -= amount; return true;
}
