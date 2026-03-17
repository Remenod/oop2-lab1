#pragma once
#include "AppState.hpp"
#include <functional>
#include <thread>

class JudgeEngine
{
public:
    // Конструктор приймає колбек для оновлення екрану (Screen::PostEvent)
    explicit JudgeEngine(std::function<void()> ui_trigger);
    ~JudgeEngine();

    void RunJudgingAsync(AppState &state);
    void StopAll();

private:
    void JudgingTask(AppState *state);

    std::function<void()> on_ui_update;
    std::thread worker_thread;
};