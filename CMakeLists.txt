cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (POLICY CMP0140)
  cmake_policy(SET CMP0140 NEW)
endif()

# if (POLICY CMP0079)
#   cmake_policy(SET CMP0079 NEW)
# endif()

set(TARGET_NAME CACodeGen)
project(CACodeGen VERSION 0.1.0)

file(GLOB_RECURSE HEADERS "Source/*.h")
file(GLOB_RECURSE SOURCES "Source/*.cpp")

add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

add_subdirectory(ExternalLibraries)
target_link_libraries(${TARGET_NAME} PRIVATE mustache)
target_link_libraries(${TARGET_NAME} PRIVATE CLANG_LIB)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${TARGET_NAME}> $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>
  COMMAND_EXPAND_LISTS
)