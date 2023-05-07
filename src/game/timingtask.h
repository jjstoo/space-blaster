#ifndef TIMINGTASK_H
#define TIMINGTASK_H

#include "SDL2/SDL.h"

class TimingTask {
  public:

    ///
    /// \brief Runs simple timing tasks
    /// \param delay_milliseconds time interval duration
    /// \param repeating is the timer repeating
    ///
    TimingTask(unsigned int delay_milliseconds, bool repeating = true);

    ///
    /// \brief Start the timer
    ///
    void start();

    ///
    /// \brief Set timer interval
    /// \param delay_milliseconds interval duration
    ///
    void setInterval(unsigned int delay_milliseconds);

    ///
    /// \brief Check if the timer is completed. Restarts the timer if
    /// it is set to be repeating.
    /// \return true if the timer is completed.
    ///
    bool isDue();

  private:
    unsigned int delay_;
    bool repeating_;
    bool started_ = false;
    Uint32 interval_start_;
};

#endif // TIMINGTASK_H
