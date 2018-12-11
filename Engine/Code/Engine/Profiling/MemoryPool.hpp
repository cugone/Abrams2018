#pragma once

#include <cstddef>

template<typename T, std::size_t maxSize>
class MemoryPool {
public:
	MemoryPool();
	~MemoryPool();

    [[nodiscard]] void* allocate(std::size_t size);
    void deallocate(void* ptr, std::size_t size);

protected:
private:

    int _count = 0;
    int _max = 0;
    T* _data = nullptr;
    T* _front = nullptr;
    T* _back = nullptr;
};

template<typename T, std::size_t maxSize>
[[nodiscard]] void* MemoryPool<T, maxSize>::allocate(std::size_t size) {
    auto elems = static_cast<int>(size / sizeof(T));
    if(_count + elems < _max) {
        _back = _front;
        _front += elems;
        _count += elems;
        return reinterpret_cast<void*>(_back);
    }
    return nullptr;
}

template<typename T, std::size_t maxSize>
void MemoryPool<T, maxSize>::deallocate(void* ptr, std::size_t size) {
    auto elems = static_cast<int>(size / sizeof(T));
    if(0 < _count - elems) {
        _back = _front;
        _front -= elems;
        _count -= elems;
    } else {
        _front = _data;
        _back = _front;
        _count = 0;
    }
}

template<typename T, std::size_t maxSize>
MemoryPool<T, maxSize>::~MemoryPool() {
    std::free(_data);
    _data = nullptr;
    _front = nullptr;
    _back = nullptr;
    _count = 0;
    _max = 0;
}

template<typename T, std::size_t maxSize>
MemoryPool<T, maxSize>::MemoryPool() {
    _data = reinterpret_cast<T*>(std::malloc(maxSize * sizeof(T)));
    _front = _data;
    _back = _front;
    _count = 0;
    _max = maxSize;
}
