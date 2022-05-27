cmake_minimum_required(VERSION 3.21)
project(reed_solomon_encoder C)

set(CMAKE_C_STANDARD 99)

add_executable(reed_solomon_encoder main.c rs_encoder.c rs_encoder.h)
