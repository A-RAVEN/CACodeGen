set(TARGET_NAME CACodeGen)

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")

add_library(CAParserLibrary STATIC ${HEADERS} ${SOURCES})

target_link_libraries(CAParserLibrary PUBLIC mustache)
target_link_libraries(CAParserLibrary PUBLIC CLANG_LIB)
target_include_directories(CAParserLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${TARGET_NAME}> $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>
#   COMMAND_EXPAND_LISTS
# )