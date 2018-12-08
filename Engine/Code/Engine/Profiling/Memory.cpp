#include "Engine/Profiling/Memory.hpp"

#ifdef TRACK_MEMORY

void* operator new(std::size_t size) {
    if(Memory::is_enabled()) {
        return Memory::allocate(size);
    } else {
        return std::malloc(size);
    }
}

void* operator new[](std::size_t size) {
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

void operator delete[](void* ptr, std::size_t size) {
    if(Memory::is_enabled()) {
        Memory::deallocate(ptr, size);
    } else {
        std::free(ptr);
    }
}

void operator delete(void* ptr) {
    if(Memory::is_enabled()) {
        Memory::deallocate(ptr);
    } else {
        std::free(ptr);
    }
}

void operator delete[](void* ptr) {
    if(Memory::is_enabled()) {
        Memory::deallocate(ptr);
    } else {
        std::free(ptr);
    }
}
#endif

void Memory::enable(bool e) {
    active = e;
}

bool Memory::is_enabled() {
    return active;
}

void Memory::resetframecounters() {
    frameSize = 0;
    frameCount = 0;
}

void* Memory::allocate(std::size_t size) {
    if(active) {
        ++frameCount;
        frameSize += size;
        ++allocCount;
        allocSize += size;
        if(maxSize < allocSize) {
            maxSize = allocSize;
        }
        if(maxCount < allocCount) {
            maxCount = allocCount;
        }
    }
    return std::malloc(size);
}

void Memory::deallocate(void* ptr, std::size_t size) {
    if(ptr) {
        --allocCount;
        allocSize -= size;
        std::free(ptr);
    }
}

void Memory::deallocate(void* ptr) {
    if(ptr) {
        --allocCount;
        std::free(ptr);
    }
}
