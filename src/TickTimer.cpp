//
// Created by lucas on 23.05.25.
//

#include "TickTimer.h"

void TickTimer::Tick(float dt) {
    if (m_done) {
        return;
    }
    m_elapsed += dt;
    if (m_elapsed >= m_timeout) {
        if (m_loop) {
            // Loop by resetting the elapsed time
           m_elapsed = 0.0f;
        } else {
            // No loop: Disable timer
            m_done = true;
        }
        m_callback(0);
    }
}

TickTimer::TickTimer(const float timeout, const std::function<void(int)> &cb) {
    m_timeout = timeout;
    m_elapsed = 0;
    m_callback = cb;
    m_done = false;
    m_loop = false;
}

void TickTimer::SetLoop(const bool loop) {
    m_loop = loop;
}

