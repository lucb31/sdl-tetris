//
// Created by lucas on 23.05.25.
//

#include "TickTimer.h"

void TickTimer::Tick(float dt) {
    if (m_done) { return; }
    m_elapsed += dt;
    if (m_elapsed >= m_timeout) {
        m_done = true;
        m_callback(0);
    }
}

TickTimer::TickTimer(const float timeout, const std::function<void(int)> &cb) {
    m_timeout = timeout;
    m_elapsed = 0;
    m_callback = cb;
    m_done = false;
}

