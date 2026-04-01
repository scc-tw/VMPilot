# cmake/LTO.cmake — link-time optimisation
#
# Enabled by VMPILOT_ENABLE_LTO.  Only meaningful for Release / MinSizeRel.

if (VMPILOT_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _ipo_ok OUTPUT _ipo_err)
    if (_ipo_ok)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "LTO enabled")
    else()
        message(WARNING "LTO requested but not supported: ${_ipo_err}")
    endif()
endif()
