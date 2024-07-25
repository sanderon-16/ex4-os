cmake_minimum_required(VERSION 3.22)
project(ex4-os-2)

set(CMAKE_CXX_STANDARD 11)

add_executable(SimpleTest SimpleTest.cpp MemoryConstants.h PhysicalMemory.cpp PhysicalMemory.h VirtualMemory.h VirtualMemory.cpp)
