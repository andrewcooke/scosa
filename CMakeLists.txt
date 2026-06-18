
cmake_minimum_required(VERSION 3.12)
set(project_name "SCosa")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 17)

include(SuperColliderServerPlugin RESULT_VARIABLE server_plugin_found)
if(NOT server_plugin_found)
    message(FATAL_ERROR "Could not find server plugin functions module")
endif()

include(SuperColliderCompilerConfig RESULT_VARIABLE compiler_config_found)
if(NOT compiler_config_found)
    message(FATAL_ERROR "Could not find compiler config module")
endif()

# Windows - puts redistributable DLLs in install directory
include(InstallRequiredSystemLibraries)

sc_check_sc_path("${SC_PATH}")
message(STATUS "Found SuperCollider: ${SC_PATH}")
set(SC_PATH "${SC_PATH}" CACHE PATH
    "Path to SuperCollider source. Relative paths are treated as relative to this script" FORCE)

include("${SC_PATH}/SCVersion.txt")
message(STATUS "Building plugins for SuperCollider version: ${SC_VERSION}")

# set project here to avoid SCVersion.txt clobbering our version info
project(${project_name})
sc_do_initial_compiler_config() # do after setting project so compiler ID is available

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR NOT CMAKE_INSTALL_PREFIX)
    message(WARNING "No install prefix provided, defaulting to $BUILD_DIR/install")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install prefix" FORCE)
endif()

message(STATUS "Install directory set to: ${CMAKE_INSTALL_PREFIX}")

option(SUPERNOVA "Build plugins for supernova" ON)
option(SCSYNTH "Build plugins for scsynth" ON)
option(NATIVE "Optimize for native architecture" OFF)
option(STRICT "Use strict warning flags" OFF)
option(NOVA_SIMD "Build plugins with nova-simd support." ON)

if (NOVA_SIMD)
	add_definitions(-DNOVA_SIMD)
	include_directories(${SC_PATH}/external_libraries/nova-simd)
endif()

file(GLOB_RECURSE CORE_LIB_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)
add_library(scosa_core STATIC ${CORE_LIB_SOURCES})
target_include_directories(scosa_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_target_properties(scosa_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

macro(add_sc_plugin NAME)
    sc_add_server_plugin(
        "SCosa/${NAME}"
        "${NAME}"
        "plugins/${NAME}/${NAME}.cpp"
        "plugins/${NAME}/${NAME}.hpp"
        "plugins/${NAME}/${NAME}.sc"
        "plugins/${NAME}/${NAME}.schelp"
    )
    if (SCSYNTH AND TARGET ${NAME}_scsynth)
        target_link_libraries(${NAME}_scsynth PRIVATE scosa_core)
    endif()
    if (SUPERNOVA AND TARGET ${NAME}_supernova)
        target_link_libraries(${NAME}_supernova PRIVATE scosa_core)
    endif()
endmacro()

add_sc_plugin(EuCosa)
add_sc_plugin(TrigGate)
add_sc_plugin(Justo)

enable_testing()
include(FetchContent)

FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG        v2.5.2
)
FetchContent_MakeAvailable(doctest)

file(GLOB_RECURSE TEST_SOURCES 
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.hpp"
)
add_executable(scosa_tests ${TEST_SOURCES})
target_link_libraries(scosa_tests PRIVATE scosa_core doctest::doctest)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(scosa_tests)
  
message(STATUS "Generating plugin targets done")

