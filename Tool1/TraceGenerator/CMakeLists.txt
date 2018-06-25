cmake_minimum_required(VERSION 3.5)
project(TraceGenerator)
set(CMAKE_CXX_STANDARD 11)
include(FindPCAP.cmake)
set(SOURCE_FILES main.cpp PaketlossModel/BasePacketlossModel.cpp PaketlossModel/BasePacketlossModel.h PaketlossModel/MarkovModel.cpp PaketlossModel/MarkovModel.h TraceGenerator.cpp TraceGenerator.h PaketlossModel/GilbertElliot.cpp PaketlossModel/GilbertElliot.h TraceSaver.cpp TraceSaver.h PacketLossToParameterParser.cpp PacketLossToParameterParser.h Pingparser.cpp Pingparser.h)

add_executable(TraceGenerator ${SOURCE_FILES})
target_link_libraries(TraceGenerator ${PCAP_LIBRARY})