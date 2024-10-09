file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(CAMetaParser ${HEADERS} ${SOURCES})
target_link_libraries(CAMetaParser PRIVATE CAParserLibrary)
target_link_libraries(CAMetaParser PRIVATE CAMetaRuntime)
target_link_libraries(CAMetaParser PRIVATE argparse)

add_custom_command(TARGET CAMetaParser POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:CAMetaParser> $<TARGET_RUNTIME_DLLS:CAMetaParser>
  COMMAND_EXPAND_LISTS
)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/MetaParser.cmake)