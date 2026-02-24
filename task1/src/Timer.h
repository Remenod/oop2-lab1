#pragma once
#include <functional>
#include <thread>
#include <atomic>

class Timer
{
    int interval;
    std::function<void()> callback;
    std::thread worker;
    std::atomic<bool> running;

public:
    Timer(int seconds, std::function<void()> cb);
    void start();
    void stop();
    bool isRunning() const { return running; }
    ~Timer();
};