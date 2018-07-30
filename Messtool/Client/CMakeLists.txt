cmake_minimum_required(VERSION 3.5)
project(Messtool)
include(FindPCAP.cmake)
set(CMAKE_CXX_STANDARD 11)

add_executable(Client client.cpp pcapWriter.h pcapWriter.cpp)
target_link_libraries(PcapWriter ${PCAP_LIBRARY})
