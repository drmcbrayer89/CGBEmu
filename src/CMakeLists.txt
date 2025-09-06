cmake_minimum_required(VERSION 3.5)

project(GBEMU)

include_directories("../headers")

add_executable(gbemu cart.c main.c bus.c cpu.c)

