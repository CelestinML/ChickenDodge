cmake_minimum_required(VERSION 3.16)

project(SimpleGE VERSION 0.0.1 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(fmt
  GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
  GIT_TAG         7.1.3
)
FetchContent_Declare(glew
  GIT_REPOSITORY  https://github.com/Perlmint/glew-cmake.git
  GIT_TAG         glew-cmake-2.2.0
)
FetchContent_Declare(glfw
  GIT_REPOSITORY  https://github.com/glfw/glfw
  GIT_TAG         e7758c506dfcf258566f866c761c1b6bd7d98011
)
FetchContent_Declare(gsl
  GIT_REPOSITORY  https://github.com/microsoft/GSL.git
  GIT_TAG         v3.1.0
)
FetchContent_Declare(json
  GIT_REPOSITORY  https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG         v3.10.0
)
FetchContent_Declare(libpng
  GIT_REPOSITORY  https://github.com/glennrp/libpng.git
  GIT_TAG         v1.6.37
)
FetchContent_Declare(zlib
  GIT_REPOSITORY  https://github.com/madler/zlib.git
  GIT_TAG         v1.2.11
)

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
  FetchContent_Populate(fmt)
  add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(glew)
if(NOT glew_POPULATED)
  FetchContent_Populate(glew)
  set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/enable_cmp0048.cmake")
  add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
  unset(CMAKE_PROJECT_INCLUDE_BEFORE)
endif()

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(gsl)
if(NOT gsl_POPULATED)
  FetchContent_Populate(gsl)
  add_subdirectory(${gsl_SOURCE_DIR} ${gsl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

SET(SKIP_INSTALL_EXPORT TRUE)

FetchContent_GetProperties(zlib)
if(NOT zlib_POPULATED)
    FetchContent_Populate(zlib)

    set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
    set(ZLIB_LIBRARY zlibstatic)

    execute_process(
      COMMAND           ${GIT_EXECUTABLE} apply ${CMAKE_CURRENT_LIST_DIR}/cmake/zlib.patch
      WORKING_DIRECTORY ${zlib_SOURCE_DIR}
      OUTPUT_QUIET
      ERROR_QUIET
    )

    set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/enable_cmp0048.cmake")
    add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} EXCLUDE_FROM_ALL)
    unset(CMAKE_PROJECT_INCLUDE_BEFORE)
endif()

FetchContent_GetProperties(libpng)
if(NOT libpng_POPULATED)
    FetchContent_Populate(libpng)

    cmake_policy(SET CMP0077 NEW)
    set(PNG_SHARED OFF)

    add_subdirectory(${libpng_SOURCE_DIR} ${libpng_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(include)

file(GLOB_RECURSE simplege_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(simplege STATIC ${simplege_SOURCES})

target_precompile_headers(simplege PRIVATE include/simplege/pch/precomp.h)
target_compile_features(simplege PUBLIC cxx_std_20)

find_package(OpenGL REQUIRED)
add_dependencies(simplege OpenGL::GL)

include_directories(SYSTEM ${fmt_SOURCE_DIR}/include)
include_directories(SYSTEM ${glew_SOURCE_DIR}/include)
include_directories(SYSTEM ${glfw_SOURCE_DIR}/include)
include_directories(SYSTEM ${gsl_SOURCE_DIR}/include)
include_directories(SYSTEM ${libpng_SOURCE_DIR}/ ${libpng_BINARY_DIR}/)
include_directories(SYSTEM ${zlib_SOURCE_DIR}/)

find_package(Threads REQUIRED)
target_link_libraries(simplege PRIVATE Threads::Threads)
target_link_libraries(simplege PRIVATE OpenGL::GL)
target_link_libraries(simplege PUBLIC fmt::fmt)
target_link_libraries(simplege PRIVATE libglew_static)
target_link_libraries(simplege PRIVATE glfw)
target_link_libraries(simplege PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(simplege PRIVATE png_static)

if (NOT WIN32)
  target_link_libraries(simplege PRIVATE stdc++fs)
endif()

if (WIN32)
  target_link_libraries(simplege PRIVATE wsock32 ws2_32)
endif()

add_subdirectory(examples)
