cmake_minimum_required(VERSION 3.10)
project(Hyc_Tests VERSION 0.01 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE REFLEX_SRC "../lib/Re-flex/lib/*.cpp" "../lib/Re-flex/unicode/*.cpp") #Static sources

add_definitions(-DROOT=\"${CMAKE_SOURCE_DIR}\")
add_executable(Hyc_Tests ${FRONTEND_SRC} ${MIDDLEEND_SRC} ${BACKEND_SRC}
${TOOLS_SRC} ${REFLEX_SRC} ${FRONTEND_HEADERS} ${MIDDLEEND_HEADERS} ${BACKEND_HEADERS} ${TOOLS_HEADERS} tests.cpp)

source_group("Frontend\\Source Files" FILES ${FRONTEND_SRC})
source_group("Frontend\\Header Files" FILES ${FRONTEND_HEADERS})
source_group("Middleend\\Source Files" FILES ${MIDDLEEND_SRC})
source_group("Middleend\\Header Files" FILES ${MIDDLEEND_HEADERS})
source_group("Backend\\Source Files" FILES ${BACKEND_SRC})
source_group("Backend\\Header Files" FILES ${BACKEND_HEADERS})
source_group("Main" FILES tests.cpp)
source_group("Tools\\Header Files" FILES ${TOOLS_HEADERS})
source_group("Tools\\Source Files" FILES ${TOOLS_SRC})
source_group("Embedded" FILES ${REFLEX_SRC})


set_property(TARGET Hyc_Tests PROPERTY CXX_STANDARD 17)
set_property(TARGET Hyc_Tests PROPERTY CXX_STANDARD_REQUIRED ON)

source_group("Embedded" FILES ${REFLEX_SRC})
target_include_directories(Hyc_Tests PRIVATE ${TARGET_INC_DIRS})
