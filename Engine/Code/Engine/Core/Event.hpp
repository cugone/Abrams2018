#pragma once

#include <algorithm>
#include <functional>
#include <tuple>
#include <utility>
#include <vector>

template<typename... Args>
class Event {
public:
    using cb_t = std::function<void(Args...)>;
    struct event_t {
        cb_t cb{};
        std::tuple<Args...> args;
        event_t(const cb_t& cb, Args... args) : cb(cb), args(std::forward_as_tuple<Args...>(std::decay_t<Args>(args)...)) { }
        bool operator==(const event_t& rhs) {
            //Test pointer then signature
            return (this->cb.target_type().hash_code() == rhs.cb.target_type().hash_code())
                || (this->args == rhs.args);
        }
    };
    Event() = default;
    ~Event() = default;

    void Subscribe(const cb_t& cb, Args... args) {
        event_t sub(cb, args...);
        Subscribe(sub);
    }

    void Unsubscribe(const cb_t& cb) {
        _subscribers.erase(
            std::remove_if(
                std::begin(_subscribers),
                std::end(_subscribers),
                [&cb](const event_t& e) {
                    return (cb.target_type().hash_code() == e.cb.target_type().hash_code());
                }),
            std::end(_subscribers));
    }

    void Trigger() const {
        for(auto& s : _subscribers) {
            std::apply(s.cb, s.args);
        }
    }

protected:
private:
    std::vector<event_t> _subscribers{};

    void Subscribe(const event_t& cb) {
        auto found = std::find(std::begin(_subscribers), std::end(_subscribers), cb);
        if(found == std::end(_subscribers)) {
            _subscribers.push_back(cb);
        }
    }

    void Unsubscribe(const event_t& cb) {
        auto found = std::find(std::begin(_subscribers), std::end(_subscribers), cb);
        if(found != std::end(_subscribers)) {
            std::iter_swap(found, _subscribers.end() - 1);
            _subscribers.pop_back();
        }
    }

};
