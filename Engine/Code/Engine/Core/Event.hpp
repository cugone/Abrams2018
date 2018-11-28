#pragma once

#include <algorithm>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

template<typename... Args>
class Event {
public:
    using cb_t = std::function<void(Args...)>;
    struct event_t {
        explicit event_t(const cb_t& cb, void* user_data) : cb(cb), user_data(user_data) {}
        ~event_t() = default;
        event_t(const event_t& other) = default;
        event_t& operator=(const event_t& rhs) = default;
        event_t(event_t&& other) = default;
        event_t& operator=(event_t&& rhs) = default;
        bool operator==(const event_t& rhs) {
            return (this->user_data == rhs.user_data);
        }
        cb_t cb{};
        void* user_data = nullptr;
    };
    Event() = default;
    ~Event() = default;

    void Subscribe(const cb_t& cb, void* user_data = nullptr) {
        Subscribe(std::move(event_t( cb, user_data )));
    }

    void Unsubscribe(const cb_t& cb, void* user_data = nullptr) {
        Unsubscribe(std::move(event_t(cb, user_data)));
    }

    void Trigger(Args&&... args) const {
        for(auto& s : _subscribers) {
            std::invoke(s.cb, args...);
        }
    }

protected:
private:
    std::vector<event_t> _subscribers{};

    void Subscribe(event_t&& cb) {
        _subscribers.push_back(cb);
    }

    void Unsubscribe(event_t&& cb) {
        auto found = std::find(std::begin(_subscribers), std::end(_subscribers), cb);
        if(found != std::end(_subscribers)) {
            std::iter_swap(found, _subscribers.end() - 1);
            _subscribers.pop_back();
        }
    }

};

template<>
class Event<void> : public Event<> {};
