option(BUILD_TESTS "Build tests" ON)

find_package(Catch2 3 CONFIG REQUIRED)

FILE(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp)

add_executable(${PROJECT_NAME}_tests
    ${TEST_SOURCES}
)

target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    Catch2::Catch2WithMain
    ${PROJECT_NAME}::${PROJECT_NAME}
)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME}_tests)