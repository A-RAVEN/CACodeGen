file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")

add_library(CAReflectionRuntime STATIC ${HEADERS} ${SOURCES})
target_include_directories(CAReflectionRuntime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(CAReflectionRuntime PRIVATE CAMetaRuntime)