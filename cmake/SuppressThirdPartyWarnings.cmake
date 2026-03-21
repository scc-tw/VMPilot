# cmake/SuppressThirdPartyWarnings.cmake
#
# Provides suppress_thirdparty_warnings(target) — adds per-compiler flags
# to suppress warnings from transitively-included abseil/protobuf headers.

function(suppress_thirdparty_warnings target)
    if (MSVC)
        target_compile_options(${target} PRIVATE /WX-)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        target_compile_options(${target} PRIVATE
            -Wno-error
            -Wno-pedantic
            -Wno-nullability-extension
            -Wno-variadic-macro-arguments-omitted
            -Wno-gcc-compat
            -Wno-gnu-anonymous-struct
            -Wno-nested-anon-types
            -Wno-sign-compare
        )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target} PRIVATE
            -Wno-error
            -Wno-pedantic
            -Wno-nullability-extension
            -Wno-gcc-compat
            -Wno-gnu-anonymous-struct
            -Wno-nested-anon-types
            -Wno-sign-compare
        )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target} PRIVATE
            -Wno-error
            -Wno-pedantic
            -Wno-overflow
            -Wno-sign-compare
        )
    endif()
endfunction()
