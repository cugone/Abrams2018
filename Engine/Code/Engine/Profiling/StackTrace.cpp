#include "Engine/Profiling/StackTrace.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Win.hpp"

#include <functional>
#include <new>

#ifdef PROFILE_BUILD
#include <DbgHelp.h>

constexpr auto MAX_FILENAME_LENGTH = 1024;
constexpr auto MAX_SYMBOL_NAME_LENGTH = 128;
constexpr auto MAX_DEPTH = 128;
constexpr auto SYMBOL_INFO_SIZE = sizeof(SYMBOL_INFO);

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

StackTrace::StackTrace()
    : StackTrace(0ul, 30ul)
{
    /* DO NOTHING */
}

StackTrace::StackTrace([[maybe_unused]]unsigned long framesToSkip,
                       [[maybe_unused]]unsigned long framesToCapture) {
#ifdef PROFILE_BUILD
    if(!_refs) {
        ++_refs;
        Initialize();
    }
    _frames.resize(framesToCapture);
    _frame_count = ::CaptureStackBackTrace(1 + framesToSkip, framesToCapture, _frames.data(), &_hash);
    
#else
    DebuggerPrintf("StackTrace unavailable. Attempting to call StackTrace in non-profile build. \n");
#endif
}

StackTrace::~StackTrace() {
    --_refs;
    if(!_refs) {
        Shutdown();
    }
}

void StackTrace::Initialize() {
#ifdef PROFILE_BUILD
    debugHelpModule = ::LoadLibraryA("DbgHelp.dll");

    LSymSetOptions = reinterpret_cast<SymSetOptions_t>(::GetProcAddress(debugHelpModule, "SymSetOptions"));
    LSymInitialize = reinterpret_cast<SymInitialize_t>(::GetProcAddress(debugHelpModule, "SymInitialize"));
    LSymFromAddr = reinterpret_cast<SymFromAddr_t>(::GetProcAddress(debugHelpModule, "SymFromAddr"));
    LSymGetLineFromAddr64 = reinterpret_cast<SymGetLineFromAddr64_t>(::GetProcAddress(debugHelpModule, "SymGetLineFromAddr64"));
    LSymCleanup = reinterpret_cast<SymCleanup_t>(::GetProcAddress(debugHelpModule, "SymCleanup"));

    LSymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    if(!LSymInitialize(process, nullptr, true)) {
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

void StackTrace::Shutdown() {
#ifdef PROFILE_BUILD
    std::free(symbol);
    symbol = nullptr;

    LSymCleanup(process);

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

bool StackTrace::operator<(const StackTrace& rhs) {
    return _hash < rhs._hash;
}
