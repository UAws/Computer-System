include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/includes
) # including .h header in current directory (sub directory) (do not modify)

add_executable(translator # your sub-project name
        # add your source file (cpp file only)
        translator.cpp)

file(GLOB translator_libs
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/*.a
        )

target_link_libraries(translator # must identical to sub-project name
        #        ${CONAN_LIBS} # include CONAN libs which defined in root level Cmake
        ${translator_libs}
        )

add_executable(workshop11_parser
        parser.cpp)

target_link_libraries(workshop11_parser # must identical to sub-project name
        #        ${CONAN_LIBS} # include CONAN libs which defined in root level Cmake
        ${translator_libs}
        )