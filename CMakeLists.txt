cmake_minimum_required(VERSION 3.1)
project(Hyc VERSION 0.01 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SOURCES 
    src/main.cpp
    src/Lexer.cpp
    src/DebugPrint.cpp
    src/ExpressionParser.cpp)

set (HEADERS
    include/Token.h
    include/DebugPrint.h
    include/ExpressionParser.h)

file(GLOB_RECURSE REFLEX_SRC lib/reflex/*.cpp) #Static sources

add_compile_definitions(ROOT=\"${CMAKE_SOURCE_DIR}\")
add_executable(Hyc ${SOURCES} ${REFLEX_SRC} ${HEADERS})

set(SYNTAX_HEADERS 
    include/Lexer.h
    include/ExpressionParser.h
    include/Token.h
)

set(SYNTAX_SOURCES
    src/Lexer.cpp
    src/ExpressionParser.cpp
)

set (TOOLS_HEADERS
    include/DebugPrint.h
)

set(TOOLS_SOURCES
    src/DebugPrint.cpp
)


source_group("Syntactical Analysis\\Source Files" FILES ${SYNTAX_SOURCES})
source_group("Syntactical Analysis\\Header Files" FILES ${SYNTAX_HEADERS})
source_group("Main" FILES src/main.cpp)
source_group("Tools\\Header Files" FILES ${TOOLS_HEADERS})
source_group("Tools\\Source Files" FILES ${TOOLS_SOURCES})

source_group("Embedded" FILES ${REFLEX_SRC})
target_include_directories(Hyc PRIVATE include lib/reflex/include)








