# NovaBank Banking Management System (C++ / MySQL / HTML-CSS-JS)

A classroom/demo banking website with a C++ Crow REST API and MySQL storage.

## Included
- Web login (demo account 1001 / PIN 1234; account 1002 / PIN 2222)
- Dashboard, balance, deposit, withdrawal, transfer and transaction history
- C++ OOP `Account` abstract base class and `SavingsAccount` / `CurrentAccount` inheritance
- MySQL database schema and sample records
- Responsive frontend

> This is an educational demo, NOT production banking software. PINs are stored plainly and there is no production-grade session/authentication, CSRF protection, rate limiting, audit security, or TLS. Do not use real money or real credentials.

## Requirements (macOS)
Install Xcode Command Line Tools:
```bash
xcode-select --install
```
Install Homebrew if needed: https://brew.sh

Install dependencies:
```bash
brew install mysql cmake
brew install crow
```
Crow may not be available as a Homebrew formula on every setup. If `brew install crow` fails, use the CMake FetchContent option in the CMakeLists (requires internet) or install Crow headers according to https://crowcpp.org/.

## 1. Start MySQL
```bash
brew services start mysql
```
If your MySQL root has a password, remember it for DB_PASSWORD below. On a fresh Homebrew install root may have no password.

## 2. Create database + demo data
```bash
mysql -u root -p < database/schema.sql
```
If root has no password, omit `-p`:
```bash
mysql -u root < database/schema.sql
```

## 3. Build C++ backend
```bash
cd backend
cmake -S . -B build
cmake --build build
```

## 4. Configure database connection
Export the password (empty if root has no password):
```bash
export DB_HOST=127.0.0.1
export DB_USER=root
export DB_PASSWORD='YOUR_MYSQL_PASSWORD'
export DB_NAME=banking_db
```

## 5. Start backend
```bash
./build/banking_server
```
It listens at http://localhost:8080.

## 6. Open website
In another terminal from project root:
```bash
cd frontend
python3 -m http.server 5500
```
Open http://localhost:5500

The frontend calls `/api/...` on its own origin. For local development, either use the C++ server's static serving/proxy or run frontend through the included simple proxy setup. Easiest is use the backend static route configured below; alternatively run `python3 -m http.server` and configure a proxy. This project includes `frontend/dev_server.py` which proxies `/api` to 8080.

Run:
```bash
cd frontend
python3 dev_server.py
```
Then visit http://localhost:5500.

## CMake
See backend/CMakeLists.txt. If Crow isn't found via package config, CMake fetches it from GitHub.

## Notes
- Demo account 1001 PIN 1234; transfer to 1002 PIN is 2222.
- Transactional updates are used for deposits/withdrawals/transfers.
- C++ service handles SQL, frontend uses fetch() calls.
