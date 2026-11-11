cmake_minimum_required(VERSION 3.16)
project(BankingManagementSystem LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG QUIET)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MYSQL REQUIRED IMPORTED_TARGET mysqlclient)

if(NOT Crow_FOUND)
  include(FetchContent)
  FetchContent_Declare(Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.1
  )
  FetchContent_MakeAvailable(Crow)
endif()

add_executable(banking_server
  src/main.cpp src/Account.cpp src/SavingsAccount.cpp src/CurrentAccount.cpp src/BankService.cpp)
target_include_directories(banking_server PRIVATE include)
target_link_libraries(banking_server PRIVATE Crow::Crow PkgConfig::MYSQL)
