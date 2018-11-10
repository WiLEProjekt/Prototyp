cmake_minimum_required(VERSION 3.10)
project(PcapReader)
include(FindPCAP.cmake)
set(CMAKE_CXX_STANDARD 17)

add_executable(PcapReader main.cpp)
target_link_libraries(PcapReader ${PCAP_LIBRARY})
