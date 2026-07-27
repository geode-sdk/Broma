cmake_minimum_required(VERSION 3.21)
project(Broma LANGUAGES C CXX)

include(cmake/get_cpm.cmake)

CPMAddPackage("gh:taocpp/PEGTL#3.2.8")
CPMAddPackage("gh:geode-sdk/result@1.4.1")

add_library(Broma ${CMAKE_CURRENT_SOURCE_DIR}/src/broma.cpp)

target_compile_features(Broma PUBLIC cxx_std_20)
target_include_directories(Broma PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Broma PUBLIC taocpp::pegtl GeodeResult)

add_executable(Broma-test ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp)
target_link_libraries(Broma-test PRIVATE Broma)
