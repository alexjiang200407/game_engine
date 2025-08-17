
require_env_var(FXC_EXE "FXC (fxc.exe) is an offline tool for compiling HLSL shaders usually found in Windows SDK directory.")

file(GLOB_RECURSE SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.hlsl")

set(COMPILED_SHADERS "")

foreach(SHADER ${SHADERS})
    # Get filename without extension
    get_filename_component(NAME_WE ${SHADER} NAME_WE)
    
    # Decide shader type by convention in filename
    if(NAME_WE MATCHES "vs.*")
        set(TARGET_PROFILE vs_5_0)
    elseif(NAME_WE MATCHES "ps.*")
        set(TARGET_PROFILE ps_5_0)
    else()
        message(FATAL_ERROR "Cannot determine shader type for ${NAME_WE}")
    endif()

    # Output .cso file
    set(OUTPUT_CSO "${CMAKE_BINARY_DIR}/shaders/${NAME_WE}.cso")
    message(STATUS ${SHADER})
    add_custom_command(
        OUTPUT ${OUTPUT_CSO}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/shaders"
        COMMAND "${FXC_EXE}" /T ${TARGET_PROFILE} /E main /Fo "${OUTPUT_CSO}" "${SHADER}"
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${NAME_WE}.hlsl -> ${NAME_WE}.cso"
    )

    list(APPEND COMPILED_SHADERS ${OUTPUT_CSO})
endforeach()

add_custom_target(Shaders ALL DEPENDS ${COMPILED_SHADERS})
