cmake_minimum_required(VERSION 3.5)
project(Pingparser)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)

find_package( PCAP REQUIRED )

include_directories( ${pcap_INCLUDE_DIRS} )

add_executable(Pingparser main.cpp)
target_link_libraries( main ${PCAP_LIBRARY} )
