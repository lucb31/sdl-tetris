//
// Created by lucas on 23.05.25.
//

#ifndef TICKTIMER_H
#define TICKTIMER_H

#include <functional>

class TickTimer {
private:
    bool m_done;
    bool m_loop;
    float m_elapsed;
    float m_timeout;
    std::function<void(int)> m_callback;

public:
    void Tick(float dt);
    TickTimer(float timeout, const std::function<void(int)> &cb);
    void SetLoop(bool loop);
};


#endif //TICKTIMER_H
