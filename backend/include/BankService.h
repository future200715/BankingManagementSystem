#pragma once
#include <string>
#include <vector>
#include <mysql/mysql.h>

class BankService {
    MYSQL* db_;
    void queryOrThrow(const std::string& sql);
public:
    BankService();
    ~BankService();
    BankService(const BankService&) = delete;
    BankService& operator=(const BankService&) = delete;

    std::string login(int accountNo, const std::string& pin);
    std::string dashboard(int accountNo);
    std::string transactions(int accountNo);
    std::string deposit(int accountNo, double amount);
    std::string withdraw(int accountNo, double amount);
    std::string transfer(int from, int to, double amount);
};
