cmake_minimum_required(VERSION 3.8)
project(TraceGenerator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp PaketlossModel/PaketlossModelType.cpp PaketlossModel/PacketlossModel.cpp PaketlossModel/PacketlossModel.h PaketlossModel/PaketlossModelType.h PaketlossModel/MarkovModel.cpp PaketlossModel/MarkovModel.h TraceGenerator.cpp TraceGenerator.h PaketlossModel/GilbertElliot.cpp PaketlossModel/GilbertElliot.h TraceSaver.cpp TraceSaver.h)
add_executable(TraceGenerator ${SOURCE_FILES})