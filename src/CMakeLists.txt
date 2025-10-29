cmake_minimum_required(VERSION 3.5)

project(GBEMU)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

include_directories("../headers")

add_executable(gbemu cart.c main.c bus.c cpu.c gb.c asm_func.c memory.c stack.c)

target_link_libraries(gbemu PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)