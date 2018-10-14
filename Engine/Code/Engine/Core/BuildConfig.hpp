
#ifndef ABRAMS2018_BUILDCONFIG_CPP
#define ABRAMS2018_BUILDCONFIG_CPP

#define TRACK_MEMORY_BASIC (0)
#define TRACK_MEMORY_VERBOSE (1)

#ifdef _DEBUG
    #define TRACK_MEMORY TRACK_MEMORY_VERBOSE
    #define AUDIO_DEBUG
    #define RENDER_DEBUG
    #undef PROFILE_BUILD
#elif defined(FINAL_BUILD)
    #undef TRACK_MEMORY
    #undef PROFILE_BUILD
#else
    #define TRACK_MEMORY TRACK_MEMORY_BASIC
    #define PROFILE_BUILD
#endif

#define MAX_LOGS 3u

#define TOKEN_PASTE_SIMPLE(x,y) x##y
#define TOKEN_PASTE(x,y) TOKEN_PASTE_SIMPLE(x,y)
#define TOKEN_STRINGIZE_SIMPLE(x) #x
#define TOKEN_STRINGIZE(x) TOKEN_STRINGIZE_SIMPLE(x)

#ifdef _WIN64
    #ifndef PLATFORM_WINDOWS
        #define PLATFORM_WINDOWS
    #endif

    #ifdef PROFILE_BUILD
        #define MAX_PROFILE_HISTORY 0xFFull
        #define MAX_PROFILE_TREES 50ull
    #endif
#elif _WIN32
    #ifndef PLATFORM_WINDOWS
        #define PLATFORM_WINDOWS
    #endif
    #ifdef PROFILE_BUILD
        #define MAX_PROFILE_HISTORY 0xFFu
        #define MAX_PROFILE_TREES 50u
    #endif
#endif

#endif