cmake_minimum_required(VERSION 3.5)
project(Messtool)
set(CMAKE_CXX_STANDARD 11)
include(FindPCAP.cmake)
add_executable(PcapWriter PcapWriter.cpp PcapWriter.h main.cpp)

add_executable(PcapWriter PcapWriter.cpp PcapWriter.h main.cpp)

target_link_libraries(PcapWriter ${PCAP_LIBRARY})
target_link_libraries(PcapWriter pthread)