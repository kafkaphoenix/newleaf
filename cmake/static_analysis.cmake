option(ENABLE_CLANG_TIDY "enable static analysis with clang-tidy" OFF)
option(ENABLE_CPPCHECK "enable static analysis with cppcheck" OFF)

# Linters (Needs compilation database sadly no MSVC support yet, change to GCC to use)
# https://stackoverflow.com/questions/65544093/no-compile-commands-json-file-using-cmake-with-vs-code-on-windows
find_program(CLANG_TIDY NAMES "clang-tidy")

if(CLANG_TIDY AND ENABLE_CLANG_TIDY AND NOT GCC) # GCC doesn't support clang-tidy with precompiled headers
    add_custom_target(
        run_clang_tidy
        COMMAND ${CLANG_TIDY} ${CMAKE_SOURCE_DIR}/src/* -p ${CMAKE_BINARY_DIR}/compile_commands.json
        COMMENT "running clang-tidy static analysis"
    )

    add_dependencies(${PROJECT_NAME} run_clang_tidy)
endif()

find_program(CPPCHECK NAMES "cppcheck")

if(CPPCHECK AND ENABLE_CPPCHECK)
    set(CPPCHECK_SOURCES
        ${CMAKE_SOURCE_DIR}/src
    )

    set(CPPCHECK_OPTIONS
        "--template=gcc"
        "--platform=win64"
        "--std=c++2b"
        # "--project=${CMAKE_BINARY_DIR}\\compile_commands.json"
        "--enable=style,performance,portability"
        "--inline-suppr"
        "-I${CMAKE_SOURCE_DIR}\\build\\*"
        "-i${CMAKE_SOURCE_DIR}\\third_party\\*"
        "--suppress=*:*\\${PROJECT_NAME}\\third_party\\*:*"
        "--suppress=*:*\\${PROJECT_NAME}\\build\\*:*"
    )

    add_custom_target(
        run_cppcheck
        COMMAND ${CPPCHECK} ${CPPCHECK_OPTIONS} ${CPPCHECK_SOURCES}
        COMMENT "running cppcheck static analysis"
    )
    add_dependencies(${PROJECT_NAME} run_cppcheck)
endif()