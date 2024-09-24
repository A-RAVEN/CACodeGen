file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(CACodeGenTest ${HEADERS} ${SOURCES})
target_link_libraries(CACodeGenTest PRIVATE CAParserLibrary)
target_link_libraries(CACodeGenTest PRIVATE CAMetaRuntime)
target_link_libraries(CACodeGenTest PRIVATE CAReflectionRuntime)
add_custom_command(TARGET CACodeGenTest POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:CACodeGenTest> $<TARGET_RUNTIME_DLLS:CACodeGenTest>
  COMMAND_EXPAND_LISTS
)