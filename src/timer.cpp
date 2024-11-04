#include "timer.h"

#include <sys/types.h>

Timer::Timer()
    : running(false)
    , paused(false)
    , elapsedTime(0)
{}

Timer::~Timer()
{
    stop();
}

void Timer::start()
{
    if (running) {
        stop();
    }

    running = true;
    paused = false;
    elapsedTime = 0;
    startTime = std::chrono::steady_clock::now();

    timerThread = std::thread([this]() {
        while (running) {
            if (!paused) {
                updateElapsedTime();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}

void Timer::pause()
{
    if (running && !paused) {
        paused = true;
        pauseStartTime = std::chrono::steady_clock::now();
    }
}

void Timer::resume()
{
    if (running && paused) {
        paused = false;
        auto pauseEndTime = std::chrono::steady_clock::now();
        auto pauseDuration = std::chrono::duration_cast<std::chrono::milliseconds>(pauseEndTime
                                                                                   - pauseStartTime)
                                 .count();
        startTime += std::chrono::milliseconds(pauseDuration);
    }
}

void Timer::stop()
{
    running = false;
    if (timerThread.joinable()) {
        timerThread.join();
    }
    updateElapsedTime();
}

bool Timer::isRunning() const
{
    return running;
}

bool Timer::isPaused() const
{
    return paused;
}

int64_t Timer::getElapsedTime() const
{
    return elapsedTime;
}

void Timer::updateElapsedTime()
{
    if (running && !paused) {
        auto currentTime = std::chrono::steady_clock::now();
        elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime)
                          .count();
    }
}
