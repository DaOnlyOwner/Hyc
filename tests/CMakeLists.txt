cmake_minimum_required(VERSION 3.10)
project(Hyc_Tests VERSION 0.01 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE REFLEX_SRC "../lib/Re-flex/lib/*.cpp" "../lib/Re-flex/unicode/*.cpp") #Static sources

add_definitions(-DROOT=\"${CMAKE_SOURCE_DIR}\")
add_executable(Hyc_Tests ${SOURCES} ${REFLEX_SRC} ${HEADERS} tests.cpp)

source_group("Syntactical Analysis\\Source Files" FILES ${SYNTAX_SOURCES})
source_group("Syntactical Analysis\\Header Files" FILES ${SYNTAX_HEADERS})
source_group("Semantic Analysis\\Source Files" FILES ${SEMANTIC_SOURCES})
source_group("Semantic Analysis\\Header Files" FILES ${SEMANTIC_HEADERS})
source_group("Code Generation\\Source Files" FILES ${CODE_GEN_SOURCES})
source_group("Code Generation\\Header Files" FILES ${CODE_GEN_HEADERS})
source_group("Main" FILES tests.cpp ../include/Pipeline.h ../src/Pipeline.cpp)
source_group("Tools\\Header Files" FILES ${TOOLS_HEADERS})
source_group("Tools\\Source Files" FILES ${TOOLS_SOURCES})

set_property(TARGET Hyc_Tests PROPERTY CXX_STANDARD 17)
set_property(TARGET Hyc_Tests PROPERTY CXX_STANDARD_REQUIRED ON)

source_group("Embedded" FILES ${REFLEX_SRC})
target_include_directories(Hyc_Tests PRIVATE "../include" "../lib/Re-flex/include")
