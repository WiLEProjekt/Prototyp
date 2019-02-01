cmake_minimum_required(VERSION 3.5)
project(TraceGenerator)
set(CMAKE_CXX_STANDARD 11)
include(FindPCAP.cmake)
set(SOURCE_FILES main.cpp Packetloss/PaketlossModel/BasePacketlossModel.cpp Packetloss/PaketlossModel/BasePacketlossModel.h Packetloss/PaketlossModel/MarkovModel.cpp Packetloss/PaketlossModel/MarkovModel.h TraceGenerator.cpp TraceGenerator.h Packetloss/PaketlossModel/GilbertElliotModel.cpp Packetloss/PaketlossModel/GilbertElliotModel.h TraceSaver.cpp TraceSaver.h Pingparser.cpp Pingparser.h PcapParser/PcapParser.cpp PcapParser/PcapParser.h PcapParser/Result.h PcapParser/PcapValues.h Packetloss/PacketLossModelType.h PcapParser/ResultPoint.h)

add_executable(TraceGenerator ${SOURCE_FILES})
target_link_libraries(TraceGenerator ${PCAP_LIBRARY})