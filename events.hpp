#pragma once

#include <vector>


template <class Event, typename ...Args>
class SingletonEvent {
private:
    static SingletonEvent<Event, Args...>* instance;
    std::vector<std::function<void(Args...)>> listeners;

public:
    ~SingletonEvent() { delete instance; }

    static SingletonEvent<Event, Args...>* Instance() {
        if(instance) return instance;
        instance = new SingletonEvent<Event, Args...>;
        return instance;
    }

    void Register(std::function<void(Args...)> listener) {
        listeners.push_back(listener);
    }

    void Fire(Args... args) {
        for(auto& listener: listeners)
            listener(args...);
    }
};

template<class Event, typename ...Args>
SingletonEvent<Event, Args...>* SingletonEvent<Event, Args...>::instance = nullptr;


class HeightmapsUnzippedEvent {};
class HeightmapsDownloadProgressEvent {};
class HeightmapDownloadedEvent {};
class FramebufferSizeChangedEvent {};
