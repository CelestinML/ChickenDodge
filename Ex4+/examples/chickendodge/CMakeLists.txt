include_directories(include)

file(GLOB_RECURSE chickendodge_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(chickendodge ${chickendodge_SOURCES})
target_precompile_headers(chickendodge PRIVATE include/chickendodge/pch/precomp.h)
target_compile_features(chickendodge PUBLIC cxx_std_20)

add_dependencies(chickendodge simplege)
target_link_libraries(chickendodge simplege)

add_custom_command(TARGET chickendodge POST_BUILD
COMMAND ${CMAKE_COMMAND} -E create_symlink
${CMAKE_SOURCE_DIR}/examples/chickendodge/data $<TARGET_FILE_DIR:chickendodge>/data)