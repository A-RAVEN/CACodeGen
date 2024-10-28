file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")

add_library(CAParserLibrary SHARED ${HEADERS} ${SOURCES})
set_target_properties(CAParserLibrary PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

target_link_libraries(CAParserLibrary PUBLIC mustache)
target_link_libraries(CAParserLibrary PUBLIC CLANG_LIB)
target_include_directories(CAParserLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

function(link_target_ca_generator target generator)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(gen_name -Wl,-force_load,$<TARGET_FILE:${generator}> PARENT_SCOPE)
  elseif(MSVC)
    # In MSVC, we will add whole archive in default.
    set(gen_name -WHOLEARCHIVE:$<TARGET_FILE:${generator}> PARENT_SCOPE)
  else()
    # Assume everything else is like gcc
    set(gen_name -Wl,--whole-archive ${generator} -Wl,--no-whole-archive PARENT_SCOPE)
  endif()
  target_link_libraries(${target} PRIVATE ${gen_name})
endfunction()