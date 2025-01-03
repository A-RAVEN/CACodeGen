function(meta_parser target_name workspace_dir)
    add_custom_command(
        #PRE_BUILD
        OUTPUT ${workspace_dir}/CodeGen/CodeGen.cpp
        COMMENT "parse meta for ${target_name}"
        COMMENT "--cppstd=20 --workspace=${workspace_dir} --includes=$<JOIN:$<TARGET_PROPERTY:${target_name},INCLUDE_DIRECTORIES>, > --modules=$<TARGET_FILE_DIR:CAMetaParser> --outdir=$<TARGET_FILE_DIR:${target_name}>"
        COMMAND CAMetaParser
        "--project=${target_name}"
        "--cppstd=20"
        "--workspace=${workspace_dir}"
        "--includes=$<JOIN:$<TARGET_PROPERTY:${target_name},INCLUDE_DIRECTORIES>,;--includes=>"
        "--modules=$<TARGET_FILE_DIR:CAMetaParser>"
        "--outdir=$<TARGET_FILE_DIR:${target_name}>"
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
    #aux_source_directory(${workspace_dir}/CodeGen CODEGEN_SOURCES)
    target_include_directories(${target_name} PRIVATE "${workspace_dir}")
    target_sources(${target_name} PRIVATE "${workspace_dir}/CodeGen/CodeGen.cpp")
    #target_sources(${target_name} PRIVATE ${CODEGEN_SOURCES})
    target_link_libraries(${target_name} PRIVATE CAMetaRuntime)
    target_compile_definitions(${target_name} PUBLIC _CAMETA_DLLEXPORT)
endfunction()