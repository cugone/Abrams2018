#pragma once

#include <type_traits>

enum class RHIInstanceType : unsigned char {
    NONE
    ,DIRECTX
    ,OPENGL
};

enum class RHIDeviceType: unsigned char {
    NONE
    ,DIRECTX
    ,OPENGL
};

enum class RHIOutputMode : unsigned int {
    FIRST_
    , WINDOWED = FIRST_
    , BORDERLESS
    , FULLSCREEN_WINDOW
    , FULLSCREEN_DEDICATED  // Dedicated Fullscreen Mode.
    , LAST_
};

RHIOutputMode& operator++(RHIOutputMode& mode);
RHIOutputMode operator++(RHIOutputMode& mode, int);
