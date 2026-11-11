#include "../include/BankService.h"
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <iostream>

static std::string env(const char* key, const char* fallback) {
 const char* v=std::getenv(key); return v ? v : fallback;
}
static std::string esc(MYSQL* db, const std::string& s) {
 std::string out(s.size()*2+1,'\\0');
 unsigned long n=mysql_real_escape_string(db,out.data(),s.c_str(),s.size());
 out.resize(n); return out;
}
static std::string money(double v) {
 std::ostringstream o; o<<std::fixed<<std::setprecision(2)<<v; return o.str();
}
BankService::BankService():db_(mysql_init(nullptr)) {
 if(!db_) throw std::runtime_error("MySQL initialization failed");
 if(!mysql_real_connect(db_,env("DB_HOST","127.0.0.1").c_str(),
   env("DB_USER","root").c_str(),env("DB_PASSWORD","").c_str(),
   env("DB_NAME","banking_db").c_str(),0,nullptr,0)) {
   std::string e=mysql_error(db_); mysql_close(db_); db_=nullptr;
   throw std::runtime_error("MySQL connection failed: "+e);
 }
}
BankService::~BankService(){ if(db_) mysql_close(db_); }
void BankService::queryOrThrow(const std::string& sql) {
 if(mysql_query(db_,sql.c_str())) throw std::runtime_error(mysql_error(db_));
}
std::string BankService::login(int accountNo,const std::string& pin) {
 std::string sql="SELECT account_no FROM accounts WHERE account_no="+std::to_string(accountNo)+
 " AND pin='"+esc(db_,pin)+"'";
 queryOrThrow(sql); MYSQL_RES* r=mysql_store_result(db_);
 bool ok=r && mysql_num_rows(r)>0; if(r) mysql_free_result(r);
 if(!ok) throw std::runtime_error("Invalid account number or PIN");
 return "{\"ok\":true}";
}
std::string BankService::dashboard(int accountNo) {
 queryOrThrow("SELECT holder_name,account_type,balance FROM accounts WHERE account_no="+std::to_string(accountNo));
 MYSQL_RES* r=mysql_store_result(db_); MYSQL_ROW row=r?mysql_fetch_row(r):nullptr;
 if(!row){if(r)mysql_free_result(r);throw std::runtime_error("Account not found");}
 std::string holder=row[0]?row[0]:"", type=row[1]?row[1]:"", bal=row[2]?row[2]:"0";
 std::string out="{\"accountNo\":"+std::to_string(accountNo)+",\"holder\":\""+esc(db_,holder)+
 "\",\"type\":\""+esc(db_,type)+"\",\"balance\":"+bal+"}";
 mysql_free_result(r); return out;
}
std::string BankService::transactions(int accountNo) {
 queryOrThrow("SELECT txn_type,amount,details,created_at FROM transactions WHERE account_no="+
 std::to_string(accountNo)+" ORDER BY id DESC LIMIT 30");
 MYSQL_RES* r=mysql_store_result(db_); std::ostringstream o; o<<"{\"transactions\":[";
 bool first=true; MYSQL_ROW row;
 while(r&&(row=mysql_fetch_row(r))) {
  if(!first)o<<","; first=false;
  o<<"{\"type\":\""<<(row[0]?row[0]:"")<<"\",\"amount\":"<<(row[1]?row[1]:"0")
   <<",\"details\":\""<<esc(db_,row[2]?row[2]:"")<<"\",\"date\":\""<<(row[3]?row[3]:"")<<"\"}";
 }
 if(r)mysql_free_result(r); o<<"]}"; return o.str();
}
std::string BankService::deposit(int accountNo,double amount) {
 if(amount<=0) throw std::runtime_error("Amount must be greater than zero");
 queryOrThrow("START TRANSACTION");
 try {
  queryOrThrow("UPDATE accounts SET balance=balance+"+money(amount)+" WHERE account_no="+std::to_string(accountNo));
  if(mysql_affected_rows(db_)!=1) throw std::runtime_error("Account not found");
  queryOrThrow("INSERT INTO transactions(account_no,txn_type,amount,details) VALUES("+std::to_string(accountNo)+",'DEPOSIT',"+money(amount)+",'Cash deposit')");
  queryOrThrow("COMMIT");
 } catch(...) { mysql_query(db_,"ROLLBACK"); throw; }
 return "{\"ok\":true}";
}
std::string BankService::withdraw(int accountNo,double amount) {
 if(amount<=0) throw std::runtime_error("Amount must be greater than zero");
 queryOrThrow("START TRANSACTION");
 try {
  queryOrThrow("UPDATE accounts SET balance=balance-"+money(amount)+" WHERE account_no="+std::to_string(accountNo)+" AND balance>="+money(amount));
  if(mysql_affected_rows(db_)!=1) throw std::runtime_error("Account not found or insufficient balance");
  queryOrThrow("INSERT INTO transactions(account_no,txn_type,amount,details) VALUES("+std::to_string(accountNo)+",'WITHDRAW',"+money(amount)+",'Cash withdrawal')");
  queryOrThrow("COMMIT");
 } catch(...) { mysql_query(db_,"ROLLBACK"); throw; }
 return "{\"ok\":true}";
}
std::string BankService::transfer(int from,int to,double amount) {
 if(amount<=0) throw std::runtime_error("Amount must be greater than zero");
 if(from==to) throw std::runtime_error("Choose a different recipient account");
 queryOrThrow("START TRANSACTION");
 try {
  queryOrThrow("UPDATE accounts SET balance=balance-"+money(amount)+" WHERE account_no="+std::to_string(from)+" AND balance>="+money(amount));
  if(mysql_affected_rows(db_)!=1) throw std::runtime_error("Sender not found or insufficient balance");
  queryOrThrow("UPDATE accounts SET balance=balance+"+money(amount)+" WHERE account_no="+std::to_string(to));
  if(mysql_affected_rows(db_)!=1) throw std::runtime_error("Recipient account not found");
  queryOrThrow("INSERT INTO transactions(account_no,txn_type,amount,details) VALUES("+std::to_string(from)+",'TRANSFER OUT',"+money(amount)+",'Transfer to "+std::to_string(to)+"')");
  queryOrThrow("INSERT INTO transactions(account_no,txn_type,amount,details) VALUES("+std::to_string(to)+",'TRANSFER IN',"+money(amount)+",'Transfer from "+std::to_string(from)+"')");
  queryOrThrow("COMMIT");
 } catch(...) { mysql_query(db_,"ROLLBACK"); throw; }
 return "{\"ok\":true}";
}
