cmake_minimum_required(VERSION 3.10)
project(Hyc VERSION 0.01 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Hyc)

#include(CheckIPOSupported)
#check_ipo_supported(RESULT sup OUTPUT err)

#if(sup)
#   message(STATUS "LTO supported")
#   set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
#else()
#   message(STATUS "Warning: LTO not supported: ${error}")
#endif() 

file(GLOB_RECURSE REFLEX_SRC ${CMAKE_SOURCE_DIR}/lib/Re-flex/lib/*.cpp ${CMAKE_SOURCE_DIR}/lib/Re-flex/unicode/*.cpp) #Static sources

file(GLOB_RECURSE FRONTEND_HEADERS ${CMAKE_SOURCE_DIR}/include/Frontend/*.h)
file(GLOB_RECURSE MIDDLEEND_HEADERS ${CMAKE_SOURCE_DIR}/include/Middleend/*.h)
file(GLOB_RECURSE TOOLS_HEADERS ${CMAKE_SOURCE_DIR}/include/Tools/*.h)
file(GLOB_RECURSE BACKEND_HEADERS ${CMAKE_SOURCE_DIR}/include/Backend/*.h)

file(GLOB_RECURSE FRONTEND_SRC ${CMAKE_SOURCE_DIR}/src/Frontend/*.cpp)
file(GLOB_RECURSE MIDDLEEND_SRC ${CMAKE_SOURCE_DIR}/src/Middleend/*.cpp)
file(GLOB_RECURSE TOOLS_SRC ${CMAKE_SOURCE_DIR}/src/Tools/*.cpp)
file(GLOB_RECURSE BACKEND_SRC ${CMAKE_SOURCE_DIR}/src/Backend/*.cpp)
set(TARGET_INC_DIRS ${CMAKE_SOURCE_DIR}/include/Frontend ${CMAKE_SOURCE_DIR}/include/Middleend 
${CMAKE_SOURCE_DIR}/include/Backend ${CMAKE_SOURCE_DIR}/include/Tools ${CMAKE_SOURCE_DIR}/lib/Re-flex/include)

find_package(LLVM REQUIRED CONFIG)
llvm_map_components_to_libnames(LLVM_LIBS core support target)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Here: ${LLVM_DIR}")
message(STATUS "Linking to LLVM libs: ${LLVM_LIBS}")

add_subdirectory(tests)
add_subdirectory(lib/fmt)




add_definitions(-DROOT=\"${CMAKE_SOURCE_DIR}\")
separate_arguments(LLVM_DEF_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEF_LIST})
add_executable(Hyc ${FRONTEND_SRC} ${MIDDLEEND_SRC} ${BACKEND_SRC}
${TOOLS_SRC} ${REFLEX_SRC} ${FRONTEND_HEADERS} ${MIDDLEEND_HEADERS} ${BACKEND_HEADERS} ${TOOLS_HEADERS}  src/main.cpp)

set_property(TARGET Hyc PROPERTY CXX_STANDARD 17)
set_property(TARGET Hyc PROPERTY CXX_STANDARD_REQUIRED ON)


source_group("Frontend\\Source Files" FILES ${FRONTEND_SRC})
source_group("Frontend\\Header Files" FILES ${FRONTEND_HEADERS})
source_group("Middleend\\Source Files" FILES ${MIDDLEEND_SRC})
source_group("Middleend\\Header Files" FILES ${MIDDLEEND_HEADERS})
source_group("Backend\\Source Files" FILES ${BACKEND_SRC})
source_group("Backend\\Header Files" FILES ${BACKEND_HEADERS})
source_group("Main" FILES src/main.cpp)
source_group("Tools\\Header Files" FILES ${TOOLS_HEADERS})
source_group("Tools\\Source Files" FILES ${TOOLS_SRC})
source_group("Embedded" FILES ${REFLEX_SRC})

target_include_directories(Hyc PRIVATE ${TARGET_INC_DIRS})
target_include_directories(Hyc PRIVATE ${LLVM_INCLUDE_DIRS})
target_include_directories(Hyc PRIVATE "lib/cxxopts/include")
target_link_libraries(Hyc fmt)
target_link_libraries(Hyc ${LLVM_LIBS})