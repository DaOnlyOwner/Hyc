cmake_minimum_required(VERSION 3.10)
project(Hyc VERSION 0.01 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(LLVM_DIR ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)
message(${LLVM_DIR})

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
    src/SymbolTable.cpp
    src/Scopes.cpp
    src/DeclarationsCollector.cpp
    src/CodeGen.cpp
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
    include/SymbolTable.h
    include/Scopes.h
    include/Hashmap.h
    include/PerfectHashmap.h
    include/Primitive.h
    include/ValueStorage.h
    include/MetaType.h
    include/ImplicitTypes.h
    include/Precedence.h
    include/DeclarationsCollector.h
    include/CodeGen.h

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
    include/Precedence.h
)

set(SYNTAX_SOURCES
    src/Lexer.cpp
    src/Parser.cpp
    src/GraphOutput.cpp
)
    
set(SEMANTIC_SOURCES
    src/TypeChecker.cpp
    src/SymbolTable.cpp
    src/Scopes.cpp
    src/DeclarationsCollector.cpp
)

set(SEMANTIC_HEADERS
    include/TypeChecker.h
    include/SymbolTable.h
    include/Scopes.h
    include/Primitive.h
    include/MetaType.h
    include/ImplicitTypes.h
    include/DeclarationsCollector.h
)

set (TOOLS_HEADERS
    include/DebugPrint.h
    include/Hashmap.h
    include/ValueStorage.h
    include/PerfectHashmap.h
)

set(TOOLS_SOURCES
    src/DebugPrint.cpp
)

set(CODE_GEN_SOURCES
    src/CodeGen.cpp
)

set(CODE_GEN_HEADERS
    include/CodeGen.h
)


find_package(LLVM REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})


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
target_include_directories(Hyc PRIVATE include lib/reflex/include)

llvm_map_components_to_libnames(llvm_libs support core irreader)
target_include_directories(Hyc PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(Hyc ${llvm_libs})






