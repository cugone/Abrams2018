#pragma once

#include <queue>
#include <mutex>

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    //TODO: Evaluate rule of six or rule of zero.
    ~ThreadSafeQueue() noexcept;

    void push(const T& t) noexcept;
    void pop() noexcept;
    decltype(auto) size() const noexcept;
    bool empty() const noexcept;

    T& back() const noexcept;
    T& back() noexcept;
    T& front() const noexcept;
    T& front() noexcept;

protected:
private:
    mutable std::mutex _cs{};
    std::queue<T> _queue{};
};

template<typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue() noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    std::queue<T> temp{};
    _queue.swap(temp);
}

template<typename T>
void ThreadSafeQueue<T>::push(const T& t) noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    _queue.push(t);
}

template<typename T>
void ThreadSafeQueue<T>::pop() noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    _queue.pop();
}

template<typename T>
decltype(auto) ThreadSafeQueue<T>::size() const noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.size();
}

template<typename T>
bool ThreadSafeQueue<T>::empty() const noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.empty();
}

template<typename T>
T& ThreadSafeQueue<T>::back() const noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.back();
}

template<typename T>
T& ThreadSafeQueue<T>::back() noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.back();
}

template<typename T>
T& ThreadSafeQueue<T>::front() const noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.front();
}

template<typename T>
T& ThreadSafeQueue<T>::front() noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.front();
}
