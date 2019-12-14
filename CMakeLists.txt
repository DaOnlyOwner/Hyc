cmake_minimum_required(VERSION 3.10)
project(Hyc VERSION 0.01 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Hyc)

set(SOURCES 
    src/main.cpp
    src/Lexer.cpp
    src/DebugPrint.cpp
    src/Pipeline.cpp
    src/Parser.cpp
    src/GraphOutput.cpp
    src/TypeChecker.cpp
    src/Context.cpp
)

set (HEADERS
    include/Token.h
    include/DebugPrint.h
    include/GenericPrattParser.h
    include/Lexer.h
    include/Pipeline.h
    include/Parser.h
    include/Ast.h
    include/GraphOutput.h
    include/TypeChecker.h
    include/Context.h
)

file(GLOB_RECURSE REFLEX_SRC lib/reflex/*.cpp) #Static sources

add_definitions(-DROOT=\"${CMAKE_SOURCE_DIR}\")
add_executable(Hyc ${SOURCES} ${REFLEX_SRC} ${HEADERS})

set(SYNTAX_HEADERS 
    include/Lexer.h
    include/GenericPrattParser.h
    include/Token.h
    include/Lexer.h
    include/Parser.h
    include/Ast.h
    include/GraphOutput.h
)

set(SYNTAX_SOURCES
    src/Lexer.cpp
    src/Parser.cpp
    src/GraphOutput.cpp
)

set(SEMANTIC_SOURCES
    src/TypeChecker.cpp
    src/Context.cpp
)

set(SEMANTIC_HEADERS
    include/TypeChecker.h
    include/Context.h
)

set (TOOLS_HEADERS
    include/DebugPrint.h
)

set(TOOLS_SOURCES
    src/DebugPrint.cpp
)


source_group("Syntactical Analysis\\Source Files" FILES ${SYNTAX_SOURCES})
source_group("Syntactical Analysis\\Header Files" FILES ${SYNTAX_HEADERS})
source_group("Semantic Analysis\\Source Files" FILES ${SEMANTIC_SOURCES})
source_group("Semantic Analysis\\Header Files" FILES ${SEMANTIC_HEADERS})
source_group("Main" FILES src/main.cpp include/Pipeline.h src/Pipeline.cpp)
source_group("Tools\\Header Files" FILES ${TOOLS_HEADERS})
source_group("Tools\\Source Files" FILES ${TOOLS_SOURCES})

source_group("Embedded" FILES ${REFLEX_SRC})
target_include_directories(Hyc PRIVATE include lib/reflex/include)








