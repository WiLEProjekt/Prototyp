cmake_minimum_required(VERSION 3.5)
project(ModelParameteriser)
set(CMAKE_CXX_STANDARD 11)
include(FindPCAP.cmake)
set(SOURCE_FILES main.cpp Packetloss/PaketlossModel/BasePacketlossModel.cpp Packetloss/PaketlossModel/BasePacketlossModel.h Packetloss/PaketlossModel/MarkovModel.cpp Packetloss/PaketlossModel/MarkovModel.h TraceGenerator.cpp TraceGenerator.h Packetloss/PaketlossModel/GilbertElliotModel.cpp Packetloss/PaketlossModel/GilbertElliotModel.h PcapParser/PcapParser.cpp PcapParser/PcapParser.h PcapParser/Result.h PcapParser/PcapValues.h Packetloss/PacketLossModelType.h PcapParser/ResultPoint.h Packetloss/PacketlossParameterParser/BernoulliParser.cpp Packetloss/PacketlossParameterParser/BernoulliParser.h Packetloss/PacketlossParameterParser/SimpleGilbertParser.cpp Packetloss/PacketlossParameterParser/SimpleGilbertParser.h)

add_executable(ModelParameteriser ${SOURCE_FILES})
target_link_libraries(ModelParameteriser ${PCAP_LIBRARY})
