cmake_minimum_required(VERSION 3.12)
project(Messtool)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Auswertung)
include_directories(Client)
include_directories(cmake-build-debug)
include_directories(PcapWriter)
include_directories(Server)


include(PcapWriter/FindPCAP.cmake)
find_package(Threads)

SET(CMAKE_CXX_FLAGS -pthread)

add_executable(oldExecutable
        Auswertung/R-Script.R
        Auswertung/Rplot5000PP.svg
        cmake-build-debug/cmake_install.cmake
        cmake-build-debug/CMakeCache.txt
        cmake-build-debug/Makefile
        cmake-build-debug/Messtool.cbp
        PcapWriter/CMakeLists.txt
        PcapWriter/FindPCAP.cmake
        PcapWriter/main.cpp
        PcapWriter/PcapWriter.cpp
        PcapWriter/PcapWriter.h)

add_executable(Client
        Auswertung/R-Script.R
        Auswertung/Rplot5000PP.svg
        Client/client.cpp
        Client/CMakeLists.txt
        Client/FindPCAP.cmake
        cmake-build-debug/cmake_install.cmake
        cmake-build-debug/CMakeCache.txt
        cmake-build-debug/Makefile
        cmake-build-debug/Messtool.cbp
        PcapWriter/CMakeLists.txt
        PcapWriter/FindPCAP.cmake
        PcapWriter/PcapWriter.cpp
        PcapWriter/PcapWriter.h
        CMakeLists.txt Client/client.h Server/server.h MeasureBandwidth/packetPairBandwidth.h MeasureBandwidth/packetPairBandwidth.cpp MeasureSignalstrength/measureSignalstrength.h MeasureSignalstrength/measureSignalStrength.cpp generateLoad/generateLoad.h generateLoad/generateLoad.cpp consts_and_utils/consts_and_utils.h consts_and_utils/consts_and_utils.cpp)

add_executable(Server
        Auswertung/R-Script.R
        Auswertung/Rplot5000PP.svg
        cmake-build-debug/cmake_install.cmake
        cmake-build-debug/CMakeCache.txt
        cmake-build-debug/Makefile
        cmake-build-debug/Messtool.cbp
        PcapWriter/CMakeLists.txt
        PcapWriter/FindPCAP.cmake
        PcapWriter/PcapWriter.cpp
        PcapWriter/PcapWriter.h
        Server/server.cpp
        Server/CMakeLists.txt
        CMakeLists.txt
        Server/server.h generateLoad/generateLoad.h generateLoad/generateLoad.cpp consts_and_utils/consts_and_utils.h consts_and_utils/consts_and_utils.cpp)

add_executable(PcapWriter
        Auswertung/R-Script.R
        Auswertung/Rplot5000PP.svg
        cmake-build-debug/cmake_install.cmake
        cmake-build-debug/CMakeCache.txt
        cmake-build-debug/Makefile
        cmake-build-debug/Messtool.cbp
        PcapWriter/CMakeLists.txt
        PcapWriter/FindPCAP.cmake
        PcapWriter/PcapWriter.cpp
        PcapWriter/PcapWriter.h
        CMakeLists.txt)

target_link_libraries(oldExecutable ${PCAP_LIBRARY})
target_link_libraries(Client ${PCAP_LIBRARY})
target_link_libraries(Server ${PCAP_LIBRARY})
target_link_libraries(PcapWriter ${PCAP_LIBRARY})