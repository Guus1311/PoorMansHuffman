cmake_minimum_required(VERSION 3.10)
Project(Huffman C CXX)

add_subdirectory(./Source/Libraries/CompressionLib)

add_executable(Decoder ./Source/Decoder.cpp)
add_executable(Encoder ./Source/Encoder.cpp)

target_link_libraries(Decoder PUBLIC Bitio)
target_link_libraries(Encoder PUBLIC Bitio)

#target_link_libraries(Decoder PUBLIC HuffmanLib)
#target_link_libraries(Encoder PUBLIC HuffmanLib)

target_include_directories(Decoder PUBLIC  "${CMAKE_SOURCE_DIR}/Source/Libraries/CompressionLib")
target_include_directories(Encoder PUBLIC  "${CMAKE_SOURCE_DIR}/Source/Libraries/CompressionLib")