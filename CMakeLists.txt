cmake_minimum_required(VERSION 3.22)
project(nsh)
add_executable(shell src/main.cpp src/builtins.cpp src/executer.cpp src/parser.cpp)
target_include_directories(shell PRIVATE include/)
