include_directories(include)

file(GLOB_RECURSE chickendodge_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(chickendodge ${chickendodge_SOURCES})
target_precompile_headers(chickendodge PRIVATE include/chickendodge/pch/precomp.h)
target_compile_features(chickendodge PUBLIC cxx_std_20)

add_dependencies(chickendodge simplege)
target_link_libraries(chickendodge simplege)

file(GLOB_RECURSE chickendodge_server_SOURCES CONFIGURE_DEPENDS server/*.cpp src/components/*.cpp)
add_executable(chickendodge_server ${chickendodge_server_SOURCES})
target_precompile_headers(chickendodge_server PRIVATE include/chickendodge/pch/precomp.h)
target_compile_features(chickendodge_server PUBLIC cxx_std_20)

add_dependencies(chickendodge_server simplege)
target_link_libraries(chickendodge_server simplege)

add_custom_command(TARGET chickendodge POST_BUILD
COMMAND ${CMAKE_COMMAND} -E create_symlink
${CMAKE_SOURCE_DIR}/examples/chickendodge/data $<TARGET_FILE_DIR:chickendodge>/data)