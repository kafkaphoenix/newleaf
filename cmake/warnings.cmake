function(set_msvc_compile_options target)
    target_compile_options(${target} PRIVATE
        /EHs
        /nologo
        /std:c++latest
        /Od
        /utf-8
        /WX
        /Wall
        /bigobj
        /wd4996 # For deprecated functions in MSVC
    )
    target_compile_options(${target} PRIVATE /external:I ${CMAKE_SOURCE_DIR}/build /W0) # Exclude from warnings
endfunction()

function(set_gcc_clang_compile_options target)
    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Werror
        -Wa
        -mbig-obj
    )
    target_compile_options(${target} PRIVATE -isystem ${CMAKE_SOURCE_DIR}/build)  # Exclude from warnings
endfunction()

if (MSVC)
    set_msvc_compile_options(${PROJECT_NAME})
elseif(GCC OR CLANG)
    set_gcc_clang_compile_options(${PROJECT_NAME})
endif()