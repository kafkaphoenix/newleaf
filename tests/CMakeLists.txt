option(BUILD_TESTS "Build tests" ON)

find_package(Catch2 3 CONFIG REQUIRED)

FILE(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp)

add_executable(${PROJECT_NAME}_tests
    ${TEST_SOURCES}
)

target_compile_definitions(${PROJECT_NAME}_tests PRIVATE "GLFW_INCLUDE_NONE")
target_compile_definitions(${PROJECT_NAME}_tests PRIVATE "GLM_ENABLE_EXPERIMENTAL")
target_compile_definitions(${PROJECT_NAME}_tests PRIVATE "STBI_FAILURE_USERMSG")

if(ENABLE_DEBUG)
    target_compile_definitions(${PROJECT_NAME}_tests PRIVATE ENABLE_DEBUG=1)
endif()

if (MSVC)
    set_msvc_compile_options(${PROJECT_NAME}_tests)
elseif(GCC OR CLANG)
    set_gcc_clang_compile_options(${PROJECT_NAME}_tests)
endif()

target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    Catch2::Catch2WithMain
    ${PROJECT_NAME}::${PROJECT_NAME}
)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME}_tests)