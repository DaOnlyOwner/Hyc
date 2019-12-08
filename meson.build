project('Hyra','cpp', default_options : ['cpp_std=c++17'], version : '0.01')

srcHeader = files([
    'include/SymbolTable.h',
    'include/Symbol.h',
    'include/ParserHelper.h',
    'include/DebugPrint.h',
    'include/StackCollection.h',
    'include/PipelineStage_TypeChecking.h',
    'include/PipelineStage_StackCreation.h',
    'include/TypeChecker.h'])

src = files([
       'src/main.cpp',
       'src/DebugPrint.cpp',
       'src/ParserHelper.cpp',
       'src/SymbolTable.cpp',
       'src/Symbol.cpp',
       'src/StackCollection.cpp',
       'src/PipelineStage_TypeChecking.cpp',
       'src/PipelineStage_StackCreation.cpp',
       'src/TypeChecker.cpp',
       'generated/HyraBaseListener.cpp',
       'generated/HyraBaseVisitor.cpp',
       'generated/HyraLexer.cpp',
       'generated/HyraListener.cpp',
       'generated/HyraParser.cpp',
       'generated/HyraVisitor.cpp'])

incdir = include_directories(['generated','include','include/antlr4'])
cxx = meson.get_compiler('cpp') 
if get_option('buildtype').startswith('debug')
    antlr4 = cxx.find_library('antlr4-runtime', dirs : [meson.source_root() / 'lib/antlr4-debug'])
    configure_file(input : meson.source_root() / 'lib/antlr4-debug/antlr4-runtime.dll', output : 'antlr4-runtime.dll', copy: true)
    add_project_arguments('-DDEBUG',language : 'cpp')
elif get_option('buildtype').startswith('release')
    antlr4 = cxx.find_library('antlr4-runtime', dirs : [meson.source_root() / 'lib/antlr4-release'])
    configure_file(input : meson.source_root() / 'lib/antlr4-release/antlr4-runtime.dll', output : 'antlr4-runtime.dll', copy : true)
else 
    error('Target only supports debug and release builds.')
endif
executable('Hyc', src+srcHeader, include_directories : incdir, dependencies : antlr4, cpp_args : '-DSRC_ROOT="' + meson.source_root()+'"')
