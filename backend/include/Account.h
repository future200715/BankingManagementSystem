#pragma once
#include <string>

class Account {
protected:
    int id_;
    std::string holder_;
    double balance_;
public:
    Account(int id, std::string holder, double balance);
    virtual ~Account() = default;
    int id() const;
    const std::string& holder() const;
    double balance() const;
    void deposit(double amount);
    bool withdraw(double amount);
    virtual std::string type() const = 0;
    virtual double interestRate() const = 0;
};
