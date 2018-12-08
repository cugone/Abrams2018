#include "Engine/Profiling/Memory.hpp"

#ifdef TRACK_MEMORY

void* operator new(std::size_t size) {
    if(Memory::is_enabled()) {
        return Memory::allocate(size);
    } else {
        return std::malloc(size);
    }
}

void operator delete(void* ptr, std::size_t size) {
    if(Memory::is_enabled()) {
        Memory::deallocate(ptr, size);
    } else {
        std::free(ptr);
    }
}

#endif