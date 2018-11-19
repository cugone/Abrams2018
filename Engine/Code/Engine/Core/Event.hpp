#pragma once

#include <functional>
#include <vector>

template<typename... Args>
class Event {
public:

    Event() = default;
    ~Event() = default;

    template <typename F, typename... Args>
    std::size_t Subscribe(const F& f, Args&&... args) {
        subscriptions.emplace_back(std::bind(f, std::forward<Args>(args)...));
        return subscriptions.size() - 1u;
    }

    void Unsubscribe(std::size_t index) {
        subscriptions[index] = nullptr;
    }

    void Trigger() const {
        for(const auto& sub : subscriptions) {
            if(sub) {
                sub();
            }
        }
    }

private:
    std::vector<std::function<void(Args...)>> subscriptions;
};
