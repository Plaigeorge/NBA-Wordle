cmake_minimum_required(VERSION 3.27)
project(NbaWorlde)

set(CMAKE_CXX_STANDARD 17)

add_executable(NbaWorlde main.cpp
        GameLogic.h
        GUI.cpp
        GUI.h)
