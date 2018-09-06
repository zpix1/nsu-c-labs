if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Werror
            -Weverything
            -Wno-padded
            -Wno-shadow
            -Wno-language-extension-token
            -Wno-reserved-id-macro
            -pedantic
            -pedantic-errors)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # TODO
else ()
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS
            "-Werror \
            -Wall \
            -Wextra \
            -Waddress \
            -Warray-bounds \
            -Wbuiltin-macro-redefined \
            -Wconversion -Winit-self \
            -Wswitch \
            -Wunreachable-code \
            -pedantic \
            -pedantic-errors")
endif ()
