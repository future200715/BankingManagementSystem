#include "../include/BankService.h"
#include "crow.h"
#include <iostream>
#include <memory>
#include <string>

int main() {
 crow::SimpleApp app;
 CROW_ROUTE(app,"/api/health")([]{return crow::json::wvalue{{"status","ok"}};});

 CROW_ROUTE(app,"/api/login").methods(crow::HTTPMethod::POST)
 ([](const crow::request& req){
  try {
   auto j=crow::json::load(req.body); if(!j) return crow::response(400,"Invalid JSON");
   BankService bank;
   int account= j["accountNo"].i(); std::string pin=j["pin"].s();
   bank.login(account,pin); crow::response r(bank.dashboard(account)); r.add_header("Content-Type","application/json"); return r;
  } catch(const std::exception& e){return crow::response(401,e.what());}
 });

 CROW_ROUTE(app,"/api/account/<int>").methods(crow::HTTPMethod::GET)
 ([](int id){try{BankService b;crow::response r(b.dashboard(id));r.add_header("Content-Type","application/json");return r;}catch(const std::exception&e){return crow::response(404,e.what());}});

 CROW_ROUTE(app,"/api/transactions/<int>").methods(crow::HTTPMethod::GET)
 ([](int id){try{BankService b;crow::response r(b.transactions(id));r.add_header("Content-Type","application/json");return r;}catch(const std::exception&e){return crow::response(500,e.what());}});

 CROW_ROUTE(app,"/api/deposit").methods(crow::HTTPMethod::POST)
 ([](const crow::request& req){try{auto j=crow::json::load(req.body);BankService b;auto out=b.deposit(j["accountNo"].i(),j["amount"].d());return crow::response(out);}catch(const std::exception&e){return crow::response(400,e.what());}});

 CROW_ROUTE(app,"/api/withdraw").methods(crow::HTTPMethod::POST)
 ([](const crow::request& req){try{auto j=crow::json::load(req.body);BankService b;auto out=b.withdraw(j["accountNo"].i(),j["amount"].d());return crow::response(out);}catch(const std::exception&e){return crow::response(400,e.what());}});

 CROW_ROUTE(app,"/api/transfer").methods(crow::HTTPMethod::POST)
 ([](const crow::request& req){try{auto j=crow::json::load(req.body);BankService b;auto out=b.transfer(j["from"].i(),j["to"].i(),j["amount"].d());return crow::response(out);}catch(const std::exception&e){return crow::response(400,e.what());}});

 CROW_ROUTE(app,"/")([]{return crow::response( crow::status::OK);});
 app.port(8080).multithreaded().run();
}
