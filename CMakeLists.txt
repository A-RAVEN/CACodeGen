cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
# if (POLICY CMP0141)
#   cmake_policy(SET CMP0141 NEW)
#   set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
# endif()

if (POLICY CMP0140)
  cmake_policy(SET CMP0140 NEW)
endif()

project(CACodeGen VERSION 0.1.0)
#include(CMake/MetaParser.cmake)
add_subdirectory(ExternalLibraries)
add_subdirectory(ParserLibrary)
add_subdirectory(MetaRuntime)
add_subdirectory(ReflectionAndSerialize)
add_subdirectory(MetaParser)
add_subdirectory(Test)



