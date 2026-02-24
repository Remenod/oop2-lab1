#include "Timer.h"
#include <chrono>

Timer::Timer(int seconds, std::function<void()> cb)
    : interval(seconds), callback(cb), running(false) {}

void Timer::start()
{
    if (running)
        return;
    running = true;

    worker = std::thread([this]()
                         {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(interval));
            if (running) callback();
        } });
}

void Timer::stop()
{
    if (!running)
        return;
    running = false;
    if (worker.joinable())
        worker.join();
}

Timer::~Timer()
{
    stop();
}

void Timer::setInterval(int _interval)
{
    if (running)
        return;
    if (_interval)
        interval = _interval;
}