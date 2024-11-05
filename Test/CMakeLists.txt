file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")
add_library(CACodeGenTest MODULE ${HEADERS} ${SOURCES})
target_link_libraries(CACodeGenTest PRIVATE CAMetaRuntime)
target_link_libraries(CACodeGenTest PRIVATE CAReflectionRuntime)
target_link_libraries(CACodeGenTest PRIVATE argparse)
target_include_directories(CACodeGenTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/TestInclue")
meta_parser(CACodeGenTest ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET CACodeGenTest POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:CACodeGenTest> $<TARGET_RUNTIME_DLLS:CACodeGenTest>
  COMMAND_EXPAND_LISTS
)

# include(CSharpUtilities)

# file(GLOB_RECURSE CSSOURCES "*.cs")

# ADD_LIBRARY(CACodeGenTest_CS SHARED ${CSSOURCES})