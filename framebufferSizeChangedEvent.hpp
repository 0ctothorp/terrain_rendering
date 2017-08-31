#pragma once

#include <vector>


class FramebufferSizeChangeListener {
public:
    virtual void OnFramebufferSizeChange() = 0;
};

class FramebufferSizeChangedEvent {
private:
    static std::vector<FramebufferSizeChangeListener*> listeners;

public:
    static void Fire() {
        for(auto listener: listeners)
            listener->OnFramebufferSizeChange();
    }

    static void Register(FramebufferSizeChangeListener *fbSizeChangeListener) {
        listeners.push_back(fbSizeChangeListener);
    }
};