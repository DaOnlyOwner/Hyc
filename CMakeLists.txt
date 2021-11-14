cmake_minimum_required(VERSION 3.10)
project(Hyc VERSION 0.01 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Hyc)

include(CheckIPOSupported)
check_ipo_supported(RESULT sup OUTPUT err)

if(sup)
   message(STATUS "LTO supported")
   set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
   message(STATUS "Warning: LTO not supported: ${error}")
endif() 


set(SOURCES 
    ${CMAKE_SOURCE_DIR}/src/Lexer.cpp
    ${CMAKE_SOURCE_DIR}/src/DebugPrint.cpp
    ${CMAKE_SOURCE_DIR}/src/Pipeline.cpp
    ${CMAKE_SOURCE_DIR}/src/Parser.cpp
    ${CMAKE_SOURCE_DIR}/src/GraphOutput.cpp
    ${CMAKE_SOURCE_DIR}/src/TypeChecker.cpp
    ${CMAKE_SOURCE_DIR}/src/SymbolTable.cpp
    ${CMAKE_SOURCE_DIR}/src/Scopes.cpp
    ${CMAKE_SOURCE_DIR}/src/DeclarationsCollector.cpp
    ${CMAKE_SOURCE_DIR}/src/CodeGen.cpp
    ${CMAKE_SOURCE_DIR}/src/AtomicType.cpp
    ${CMAKE_SOURCE_DIR}/src/PointerType.cpp
    ${CMAKE_SOURCE_DIR}/src/Operators.cpp
)

set (HEADERS
    ${CMAKE_SOURCE_DIR}/include/Token.h
    ${CMAKE_SOURCE_DIR}/include/DebugPrint.h
    ${CMAKE_SOURCE_DIR}/include/GenericPrattParser.h
    ${CMAKE_SOURCE_DIR}/include/Lexer.h
    ${CMAKE_SOURCE_DIR}/include/Pipeline.h
    ${CMAKE_SOURCE_DIR}/include/Parser.h
    ${CMAKE_SOURCE_DIR}/include/Ast.h
    ${CMAKE_SOURCE_DIR}/include/GraphOutput.h
    ${CMAKE_SOURCE_DIR}/include/TypeChecker.h
    ${CMAKE_SOURCE_DIR}/include/SymbolTable.h
    ${CMAKE_SOURCE_DIR}/include/Scopes.h
    ${CMAKE_SOURCE_DIR}/include/Hashmap.h
    ${CMAKE_SOURCE_DIR}/include/PerfectHashmap.h
    ${CMAKE_SOURCE_DIR}/include/Primitive.h
    ${CMAKE_SOURCE_DIR}/include/ValueStorage.h
    ${CMAKE_SOURCE_DIR}/include/AtomicType.h
    ${CMAKE_SOURCE_DIR}/include/Precedence.h
    ${CMAKE_SOURCE_DIR}/include/DeclarationsCollector.h
    ${CMAKE_SOURCE_DIR}/include/CodeGen.h
    ${CMAKE_SOURCE_DIR}/include/PointerType.h
    ${CMAKE_SOURCE_DIR}/include/Operators.h
)

file(GLOB_RECURSE REFLEX_SRC "lib/Re-flex/lib/*.cpp" "lib/Re-flex/unicode/*.cpp") #Static sources

set(SYNTAX_HEADERS 
    ${CMAKE_SOURCE_DIR}/include/Lexer.h
    ${CMAKE_SOURCE_DIR}/include/GenericPrattParser.h
    ${CMAKE_SOURCE_DIR}/include/Token.h
    ${CMAKE_SOURCE_DIR}/include/Lexer.h
    ${CMAKE_SOURCE_DIR}/include/Parser.h
    ${CMAKE_SOURCE_DIR}/include/Ast.h
    ${CMAKE_SOURCE_DIR}/include/GraphOutput.h
    ${CMAKE_SOURCE_DIR}/include/Precedence.h
)

set(SYNTAX_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Lexer.cpp
    ${CMAKE_SOURCE_DIR}/src/Parser.cpp
    ${CMAKE_SOURCE_DIR}/src/GraphOutput.cpp
)
    
set(SEMANTIC_SOURCES
    ${CMAKE_SOURCE_DIR}/src/TypeChecker.cpp
    ${CMAKE_SOURCE_DIR}/src/SymbolTable.cpp
    ${CMAKE_SOURCE_DIR}/src/Scopes.cpp
    ${CMAKE_SOURCE_DIR}/src/DeclarationsCollector.cpp
    ${CMAKE_SOURCE_DIR}/src/AtomicType.cpp
    ${CMAKE_SOURCE_DIR}/src/Operators.cpp
)

set(SEMANTIC_HEADERS
    ${CMAKE_SOURCE_DIR}/include/TypeChecker.h
    ${CMAKE_SOURCE_DIR}/include/SymbolTable.h
    ${CMAKE_SOURCE_DIR}/include/Scopes.h
    ${CMAKE_SOURCE_DIR}/include/Primitive.h
    ${CMAKE_SOURCE_DIR}/include/AtomicType.h
    ${CMAKE_SOURCE_DIR}/include/DeclarationsCollector.h
    ${CMAKE_SOURCE_DIR}/include/Operators.h
)

set (TOOLS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/DebugPrint.h
    ${CMAKE_SOURCE_DIR}/include/Hashmap.h
    ${CMAKE_SOURCE_DIR}/include/ValueStorage.h
    ${CMAKE_SOURCE_DIR}/include/PerfectHashmap.h
)

set(TOOLS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/DebugPrint.cpp
)

set(CODE_GEN_SOURCES
    ${CMAKE_SOURCE_DIR}/src/CodeGen.cpp
)

set(CODE_GEN_HEADERS
    ${CMAKE_SOURCE_DIR}/include/CodeGen.h
)

add_subdirectory(tests)
add_subdirectory(lib/fmt)

add_definitions(-DROOT=\"${CMAKE_SOURCE_DIR}\")
add_executable(Hyc ${SOURCES} ${REFLEX_SRC} ${HEADERS} src/main.cpp)

set_property(TARGET Hyc PROPERTY CXX_STANDARD 17)
set_property(TARGET Hyc PROPERTY CXX_STANDARD_REQUIRED ON)

source_group("Syntactical Analysis\\Source Files" FILES ${SYNTAX_SOURCES})
source_group("Syntactical Analysis\\Header Files" FILES ${SYNTAX_HEADERS})
source_group("Semantic Analysis\\Source Files" FILES ${SEMANTIC_SOURCES})
source_group("Semantic Analysis\\Header Files" FILES ${SEMANTIC_HEADERS})
source_group("Code Generation\\Source Files" FILES ${CODE_GEN_SOURCES})
source_group("Code Generation\\Header Files" FILES ${CODE_GEN_HEADERS})
source_group("Main" FILES src/main.cpp include/Pipeline.h src/Pipeline.cpp)
source_group("Tools\\Header Files" FILES ${TOOLS_HEADERS})
source_group("Tools\\Source Files" FILES ${TOOLS_SOURCES})
source_group("Embedded" FILES ${REFLEX_SRC})

target_include_directories(Hyc PRIVATE include "lib/Re-flex/include")
target_link_libraries(Hyc fmt)