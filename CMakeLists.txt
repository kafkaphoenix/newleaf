cmake_minimum_required(VERSION 3.21)

project(newleaf
VERSION 0.1.0
HOMEPAGE_URL "https://github.com/kafkaphoenix/newleaf"
DESCRIPTION "C++ game engine for 3d and 2d development"
LANGUAGES CXX
)

message(VERBOSE "*")
message(VERBOSE "* ${PROJECT_NAME} v${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message(VERBOSE "*")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_DEBUG "enable debugging info" OFF)
option(BUILD_SHARED_LIBS "build engine as a shared library" OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/*.cpp)

if(BUILD_SHARED_LIBS)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # required when using MSVC
add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
add_library(${PROJECT_NAME} STATIC ${SOURCES})
endif()
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(${CMAKE_CURRENT_LIST_DIR}/cmake/warnings.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/static_analysis.cmake)

if(ENABLE_DEBUG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_DEBUG=1)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR/${PROJECT_NAME}}>
)

add_subdirectory(third_party)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # .lib files
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # .so files
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # .dll files
)

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)


if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

option(NEWLEAF_BUILD_DOCS "Enable building with documentation." OFF)

if(NEWLEAF_BUILD_DOCS)
    find_package(Doxygen 1.12)

    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    else()
        message(WARNING "Doxygen not found, cannot build documentation.")
    endif()
endif()