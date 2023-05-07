#include "timingtask.h"

TimingTask::TimingTask(unsigned int delay_milliseconds, bool repeating) {
    delay_ = delay_milliseconds;
    repeating_ = repeating;
}

void TimingTask::start() {
    started_ = true;
    interval_start_ = SDL_GetTicks();
}

bool TimingTask::isDue() {
    // Task not started
    if (!started_)
        return false;

    // Record current time
    Uint32 now = SDL_GetTicks();
    Uint32 interval = now - interval_start_;

    if (interval >= delay_) {
        if (repeating_) {
            // Adjust next interval if this one was checked late
            Uint32 delta = interval - delay_;
            interval_start_ = now - delta;
        }
        return true;
    }
    return false;
}

void TimingTask::setInterval(unsigned int delay_milliseconds) {
    delay_ = delay_milliseconds;
}
