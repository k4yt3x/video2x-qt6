#ifndef TIMER_H
#define TIMER_H

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

class Timer
{
public:
    Timer();
    ~Timer();

    void start();
    void pause();
    void resume();
    void stop();

    bool isRunning() const;
    bool isPaused() const;
    int64_t getElapsedTime() const;

private:
    std::atomic<bool> running;
    std::atomic<bool> paused;
    std::thread timerThread;
    int64_t elapsedTime;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point pauseStartTime;

    void updateElapsedTime();
};

#endif // TIMER_H
