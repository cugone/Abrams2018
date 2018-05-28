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
    decltype(auto) size() const;
    T& back() const;
    bool empty() const;
    T& front() const;

protected:
private:
    std::mutex _cs{};
    std::queue<T> _queue{};
    std::deque<T> _deque{};
};

template<typename T>
T& ThreadSafeQueue<T>::front() const {
    {
    std::scoped_lock<std::mutex> lock(_cs);
    auto val = _queue.front();
    }
    return val;
}

template<typename T>
bool ThreadSafeQueue<T>::empty() const {
    {
    std::scoped_lock<std::mutex> lock(_cs);
    auto val = _queue.empty();
    }
    return val;
}

template<typename T>
T& ThreadSafeQueue<T>::back() const {
    {
    std::scoped_lock<std::mutex> lock(_cs);
    auto val = _queue.back();
    }
    return val;
}

template<typename T>
decltype(auto) ThreadSafeQueue<T>::size() const {
    {
    std::scoped_lock<std::mutex> lock(_cs);
    auto val = _queue.size();
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
