# ... existing code ...
file(GLOB SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.slang")

set(WGSL_OUTPUTS "")
set(SLANG_BIN "/Users/ingun/dists/slang-macos-dist-aarch64/slangc")
set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
foreach (SHADER_FILE ${SHADER_FILES})
    # Get the filename without extension (e.g., "shader")
    get_filename_component(FILENAME_WE ${SHADER_FILE} NAME_WE)
    set(OUTPUT_FILE "${OUTPUT_DIR}/${FILENAME_WE}.wgsl")

    # Define how to generate each .wgsl file
    add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
            COMMAND ${SLANG_BIN} ${SHADER_FILE} -target wgsl -o ${OUTPUT_FILE}
            DEPENDS ${SHADER_FILE}
            COMMENT "Compiling ${SHADER_FILE} to WGSL"
            VERBATIM
    )

    list(APPEND WGSL_OUTPUTS ${OUTPUT_FILE})

    set(OUTPUT_FILE "${OUTPUT_DIR}/${FILENAME_WE}.sprv")

    # Define how to generate each .wgsl file
    add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
            COMMAND ${SLANG_BIN} ${SHADER_FILE} -target spirv -o ${OUTPUT_FILE}
            DEPENDS ${SHADER_FILE}
            COMMENT "Compiling ${SHADER_FILE} to SPIR-V"
            VERBATIM
    )

    list(APPEND WGSL_OUTPUTS ${OUTPUT_FILE})
endforeach ()

# Create a target that depends on all generated WGSL files
add_custom_target(shaders ALL
        DEPENDS ${WGSL_OUTPUTS}
)
# ... existing code ...