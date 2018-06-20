cmake_minimum_required(VERSION 3.5)
project(TraceGenerator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp PaketlossModel/PacketlossModel.cpp PaketlossModel/PacketlossModel.h PaketlossModel/MarkovModel.cpp PaketlossModel/MarkovModel.h TraceGenerator.cpp TraceGenerator.h PaketlossModel/GilbertElliot.cpp PaketlossModel/GilbertElliot.h TraceSaver.cpp TraceSaver.h RealTraceParser.cpp RealTraceParser.h PacketLossToParameterParser.cpp PacketLossToParameterParser.h HMM.cpp HMM.h)
add_executable(TraceGenerator ${SOURCE_FILES})