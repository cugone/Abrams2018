#include "Engine/Profiling/StackTrace.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileLogger.hpp"
#include "Engine/Core/Win.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <ostream>
#include <new>

#ifdef PROFILE_BUILD
#include <DbgHelp.h>

static constexpr auto MAX_FILENAME_LENGTH = 1024u;
static constexpr auto MAX_CALLSTACK_LINES = 128ul;
static constexpr auto MAX_CALLSTACK_STR_LENGTH = 2048u;
static constexpr auto MAX_SYMBOL_NAME_LENGTH = 128u;
static constexpr auto MAX_DEPTH = 128u;
static constexpr auto SYMBOL_INFO_SIZE = sizeof(SYMBOL_INFO);

struct callstack_line_t {
    char filename[MAX_CALLSTACK_STR_LENGTH];
    char function_name[MAX_CALLSTACK_STR_LENGTH];
    uint32_t line{};
    uint32_t offset{};
};

using SymSetOptions_t = bool(__stdcall *)(DWORD SymOptions);
using SymInitialize_t = bool(__stdcall *)(HANDLE hProcess, PCSTR UserSearchPath, BOOL fInvadeProcess);
using SymCleanup_t = bool(__stdcall *)(HANDLE hProcess);
using SymFromAddr_t = bool(__stdcall *)(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol);
using SymGetLineFromAddr64_t = bool(__stdcall *)(HANDLE hProcess, DWORD64 qwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line64);

static HMODULE debugHelpModule;
static HANDLE process;
static SYMBOL_INFO* symbol = nullptr;
static SymSetOptions_t LSymSetOptions;
static SymInitialize_t LSymInitialize;
static SymFromAddr_t LSymFromAddr;
static SymGetLineFromAddr64_t LSymGetLineFromAddr64;
static SymCleanup_t LSymCleanup;

#endif

std::atomic_uint64_t StackTrace::_refs = 0;
std::shared_mutex StackTrace::_cs{};
std::atomic_bool StackTrace::_did_init = false;

StackTrace::StackTrace()
    : StackTrace(1ul, 30ul)
{
    /* DO NOTHING */
}

StackTrace::StackTrace([[maybe_unused]]unsigned long framesToSkip,
                       [[maybe_unused]]unsigned long framesToCapture) {
#ifdef PROFILE_BUILD
    if(!_refs) {
        Initialize();
    }
    ++_refs;
    unsigned long count = ::CaptureStackBackTrace(1ul + framesToSkip, framesToCapture, _frames, &_hash);
    if(!count) {
        DebuggerPrintf("StackTrace unavailable. All frames were skipped. \n");
        return;
    }
    _frame_count = (std::min)(count, MAX_FRAMES_PER_CALLSTACK);
    
    callstack_line_t lines[MAX_CALLSTACK_LINES];
    const auto line_count = GetLines(this, lines, MAX_CALLSTACK_LINES);
    char line_buffer[MAX_CALLSTACK_STR_LENGTH];
    for(uint32_t i = 0u; i < line_count; ++i) {
        ::sprintf_s(line_buffer, MAX_CALLSTACK_STR_LENGTH, "\t%s(%u): %s\n",
                    lines[i].filename, lines[i].line, lines[i].function_name);
        DebuggerPrintf(line_buffer);
    }
#else
    DebuggerPrintf("StackTrace unavailable. Attempting to call StackTrace in non-profile build. \n");
#endif
}

StackTrace::~StackTrace() {
#ifdef PROFILE_BUILD
    --_refs;
    if(!_refs) {
        Shutdown();
    }
#endif
}

void StackTrace::Initialize() {
#ifdef PROFILE_BUILD
    debugHelpModule = ::LoadLibraryA("DbgHelp.dll");

    LSymSetOptions = reinterpret_cast<SymSetOptions_t>(::GetProcAddress(debugHelpModule, "SymSetOptions"));
    LSymInitialize = reinterpret_cast<SymInitialize_t>(::GetProcAddress(debugHelpModule, "SymInitialize"));
    LSymFromAddr = reinterpret_cast<SymFromAddr_t>(::GetProcAddress(debugHelpModule, "SymFromAddr"));
    LSymGetLineFromAddr64 = reinterpret_cast<SymGetLineFromAddr64_t>(::GetProcAddress(debugHelpModule, "SymGetLineFromAddr64"));
    LSymCleanup = reinterpret_cast<SymCleanup_t>(::GetProcAddress(debugHelpModule, "SymCleanup"));

    {
        std::scoped_lock<std::shared_mutex> _lock(_cs);
        LSymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    }
    process = ::GetCurrentProcess();
    {
        std::scoped_lock<std::shared_mutex> _lock(_cs);
        if(!_did_init) {
            _did_init = LSymInitialize(process, nullptr, true);
        }
    }
    if(!_did_init) {
        --_refs;
        Shutdown();
        ERROR_AND_DIE("Could not initialize StackTrace!\n");
    }
    symbol = (SYMBOL_INFO*)std::malloc(SYMBOL_INFO_SIZE + MAX_FILENAME_LENGTH * sizeof(char));
    symbol->MaxNameLen = MAX_FILENAME_LENGTH;
    symbol->SizeOfStruct = SYMBOL_INFO_SIZE;
    symbol->MaxNameLen = MAX_SYMBOL_NAME_LENGTH;
#endif
}

[[maybe_unused]] uint32_t StackTrace::GetLines([[maybe_unused]]StackTrace* st,
                                               [[maybe_unused]]callstack_line_t* lines,
                                               [[maybe_unused]]unsigned long max_lines) {
#ifdef PROFILE_BUILD
    IMAGEHLP_LINE64 line_info{};
    DWORD line_offset = 0;
    line_info.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    uint32_t count = std::min(max_lines, st->_frame_count);
    uint32_t idx = 0;

    for(uint32_t i = 0; i < count; ++i) {
        callstack_line_t* line = &(lines[idx]);
        DWORD64 ptr = reinterpret_cast<DWORD64>(st->_frames[i]);
        bool got_addr = false;
        {
            std::scoped_lock<std::shared_mutex> _lock(_cs);
            got_addr = LSymFromAddr(process, ptr, 0, symbol);
        }
        if(!got_addr) {
            continue;
        }
        ::strcpy_s(line->function_name, MAX_CALLSTACK_STR_LENGTH, symbol->Name);
        bool got_line = false;
        {
            std::scoped_lock<std::shared_mutex> _lock(_cs);
            got_line = LSymGetLineFromAddr64(process, ptr, &line_offset, &line_info);
        }
        if(got_line) {
            line->line = line_info.LineNumber;
            line->offset = line_offset;
            ::strcpy_s(line->filename, MAX_CALLSTACK_STR_LENGTH, line_info.FileName);
        } else {
            line->line = 0;
            line->offset = 0;
            ::strcpy_s(line->filename, MAX_CALLSTACK_STR_LENGTH, "N/A");
        }
        ++idx;
    }
    return idx;
#else
    return 0;
#endif
}

void StackTrace::Shutdown() {
#ifdef PROFILE_BUILD
    std::free(symbol);
    symbol = nullptr;

    {
        std::scoped_lock<std::shared_mutex> _lock(_cs);
        LSymCleanup(process);
    }

    ::FreeLibrary(debugHelpModule);
    debugHelpModule = nullptr;
#endif
}

bool StackTrace::operator!=(const StackTrace& rhs) {
    return !(*this == rhs);
}

bool StackTrace::operator==(const StackTrace& rhs) {
    return _hash == rhs._hash;
}
