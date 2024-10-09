
function(meta_parser target_name workspace_dir)
    add_custom_command(
        #PRE_BUILD
        OUTPUT ${workspace_dir}/CodeGen/CodeGen.cpp
        COMMENT "parse meta for ${target_name}"
        COMMENT "--cppstd=20 --workspace=${workspace_dir} --includes=$<JOIN:$<TARGET_PROPERTY:${target_name},INCLUDE_DIRECTORIES>, >"
        COMMAND CAMetaParser
        "--cppstd=20"
        "--workspace=${workspace_dir}"
        "--includes=$<JOIN:$<TARGET_PROPERTY:${target_name},INCLUDE_DIRECTORIES>,;--includes=>"
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
    target_include_directories(${target_name} PRIVATE "${workspace_dir}")
    target_sources(${target_name} PRIVATE "${workspace_dir}/CodeGen/CodeGen.cpp")
endfunction()