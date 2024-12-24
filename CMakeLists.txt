cmake_minimum_required(VERSION 3.11)
project(sousflow CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sousflow STATIC 
    src/sousflow.cpp
    include/sousflow.h
)