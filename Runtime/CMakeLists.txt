file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")
add_library(CAMetaRuntime STATIC ${HEADERS} ${SOURCES})
target_include_directories(CAMetaRuntime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)