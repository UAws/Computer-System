include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/includes
) # including .h header in current directory (sub directory) (do not modify)

conan_basic_setup() # conan support

#add_compile_definitions(GTEST_LOCAL_TEST)

file(GLOB cs_libs
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/*.a
        )

add_executable(parser # your sub-project name
        # add your source file (cpp file only)
        parser.cpp)

target_link_libraries(parser # must identical to sub-project name
#        ${CONAN_LIBS} # include CONAN libs which defined in root level Cmake
        ${cs_libs}
        )


add_executable(codegen
        codegen.cpp
        )

target_link_libraries(codegen ${cs_libs})

add_executable(pretty
        pretty.cpp
        )

target_link_libraries(pretty ${cs_libs})

add_executable(optimiser-r
        optimiser-r.cpp
        )


target_link_libraries(optimiser-r ${cs_libs})