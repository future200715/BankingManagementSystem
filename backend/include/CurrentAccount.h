#pragma once
#include "Account.h"
class CurrentAccount final : public Account {
public:
    using Account::Account;
    std::string type() const override;
    double interestRate() const override;
};
