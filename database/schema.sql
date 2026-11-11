CREATE DATABASE IF NOT EXISTS banking_db;
USE banking_db;

CREATE TABLE IF NOT EXISTS accounts (
  account_no INT PRIMARY KEY,
  holder_name VARCHAR(100) NOT NULL,
  account_type ENUM('Savings','Current') NOT NULL DEFAULT 'Savings',
  balance DECIMAL(13,2) NOT NULL DEFAULT 0.00,
  pin VARCHAR(20) NOT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS transactions (
  id BIGINT AUTO_INCREMENT PRIMARY KEY,
  account_no INT NOT NULL,
  txn_type VARCHAR(30) NOT NULL,
  amount DECIMAL(13,2) NOT NULL,
  details VARCHAR(255),
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  INDEX(account_no),
  CONSTRAINT fk_transaction_account FOREIGN KEY(account_no)
    REFERENCES accounts(account_no) ON DELETE CASCADE
);

-- DEMO USERS (PIN is plain text for classroom demo ONLY; never do this in production)
INSERT INTO accounts(account_no,holder_name,account_type,balance,pin) VALUES
(1001,'Aarav Sharma','Savings',25000.00,'1234'),
(1002,'Ananya Verma','Current',18000.00,'2222')
ON DUPLICATE KEY UPDATE holder_name=VALUES(holder_name);
INSERT INTO transactions(account_no,txn_type,amount,details) VALUES
(1001,'DEPOSIT',5000,'Initial demo deposit'),
(1001,'WITHDRAW',1200,'ATM withdrawal'),
(1002,'DEPOSIT',18000,'Account opening');
