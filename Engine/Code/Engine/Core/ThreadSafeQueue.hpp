#pragma once

#include <queue>
#include <mutex>

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue();

    void push(const T& t);
    void pop();
    decltype(auto) size();
    T& back();
    bool empty();
    T& front();

protected:
private:
    std::mutex _cs{};
    std::queue<T> _queue{};
};

template<typename T>
T& ThreadSafeQueue<T>::front() {
    T* val = nullptr;
    {
    std::scoped_lock<std::mutex> lock(_cs);
    val = &_queue.front();
    }
    return *val;
}

template<typename T>
bool ThreadSafeQueue<T>::empty() {
    bool val = false;
    {
    std::scoped_lock<std::mutex> lock(_cs);
    val = _queue.empty();
    }
    return val;
}

template<typename T>
T& ThreadSafeQueue<T>::back() {
    T* val = nullptr;
    {
    std::scoped_lock<std::mutex> lock(_cs);
    val = &_queue.back();
    }
    return *val;
}

template<typename T>
decltype(auto) ThreadSafeQueue<T>::size() {
    std::size_t val{};
    {
    std::scoped_lock<std::mutex> lock(_cs);
    val = _queue.size();
    }
    return val;
}

template<typename T>
void ThreadSafeQueue<T>::pop() {
    {
    std::scoped_lock<std::mutex> lock(_cs);
    _queue.pop();
    }
}

template<typename T>
void ThreadSafeQueue<T>::push(const T& t) {
    {
    std::scoped_lock<std::mutex> lock(_cs);
    _queue.push(t);
    }
}

template<typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue() {
    {
    std::scoped_lock<std::mutex> lock(_cs);
    std::queue<T> temp{};
    _queue.swap(temp);
    }
}
