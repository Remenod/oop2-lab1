#include "../include/JudgeEngine.hpp"
#include "../include/CodeGenerators.hpp"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <random>
#include <cstdlib>

template <typename Func>
long long MeasureTime(Func action)
{
    auto start = std::chrono::high_resolution_clock::now();
    action();
    auto end = std::chrono::high_resolution_clock::now();
    return duration_cast<std::chrono::milliseconds>(end - start).count();
}

void ValidateSorting(std::function<void()> ref_action, std::function<void()> stud_action,
                     int test_num, int N, const std::function<void(std::string)> &ui_cb)
{
    std::ofstream in_file("runtime/input.txt");
    in_file << N << "\n";
    std::mt19937 rng(42 + test_num);
    for (int i = 0; i < N; ++i)
        in_file << (rng() % 100000) << " ";
    in_file.close();

    long long t_eta = MeasureTime(ref_action);
    long long t_stud = MeasureTime(stud_action);

    std::ifstream out_ref("runtime/ref_out.txt");
    std::ifstream out_stud("runtime/stud_out.txt");
    bool correct = true;
    for (int i = 0; i < N; ++i)
    {
        int val_r, val_s;
        if (!(out_ref >> val_r) || !(out_stud >> val_s) || val_r != val_s)
        {
            correct = false;
            break;
        }
    }
    out_ref.close();
    out_stud.close();

    if (!correct)
    {
        ui_cb("Test " + std::to_string(test_num) + ": WRONG ANSWER");
        return;
    }

    long long lower_bound = t_eta / 2;
    long long upper_bound = (t_eta * 15 / 10) + 50;
    std::string time_str = " (Ref: " + std::to_string(t_eta) + "ms, Stud: " + std::to_string(t_stud) + "ms)";

    if (t_stud < lower_bound)
        ui_cb("Test " + std::to_string(test_num) + ": TIME ERROR (Too Fast)" + time_str);
    else if (t_stud > upper_bound)
        ui_cb("Test " + std::to_string(test_num) + ": TIME ERROR (Too Slow)" + time_str);
    else
        ui_cb("Test " + std::to_string(test_num) + ": ACCEPTED" + time_str);
}

JudgeEngine::JudgeEngine(std::function<void()> ui_trigger) : on_ui_update(ui_trigger) {}

JudgeEngine::~JudgeEngine() { StopAll(); }

void JudgeEngine::StopAll()
{
    if (worker_thread.joinable())
    {
        worker_thread.join();
    }
}

void JudgeEngine::RunJudgingAsync(AppState &state)
{
    state.ClearResults();
    StopAll();
    worker_thread = std::thread(&JudgeEngine::JudgingTask, this, &state);
}

void JudgeEngine::JudgingTask(AppState *state)
{
    auto add_result = [&](std::string msg)
    {
        state->AddResult(msg);
        on_ui_update();
    };

    std::filesystem::create_directories("runtime");

    std::string ext = (state->selected_lang == 0) ? ".c" : ((state->selected_lang == 1) ? ".cpp" : ".py");
    std::string stud_filename = "./runtime/stud_sol" + ext;
    std::string ref_filename = "./runtime/ref_sol" + ext;

    std::ofstream out_stud(stud_filename);
    out_stud << state->code_content;
    out_stud.close();

    std::ofstream out_ref(ref_filename);
    out_ref << GetReferenceCode(static_cast<SolveProblem>(state->selected_problem),
                                static_cast<SolveLanguage>(state->selected_lang));
    out_ref.close();

    if (state->selected_lang == 0 || state->selected_lang == 1)
    {
        std::string compiler = (state->selected_lang == 0) ? "gcc" : "g++";
        if (std::system((compiler + " " + stud_filename + " -o runtime/stud_sol.out").c_str()) != 0)
        {
            add_result("Result: COMPILATION ERROR");
            state->SetFinished(true);
            on_ui_update();
            return;
        }
        std::system((compiler + " " + ref_filename + " -o runtime/ref_sol.out").c_str());
    }

    auto cmd_stud = (state->selected_lang == 2)
                        ? "python3 runtime/stud_sol.py < runtime/input.txt > runtime/stud_out.txt"
                        : "./runtime/stud_sol.out < runtime/input.txt > runtime/stud_out.txt";
    auto cmd_ref = (state->selected_lang == 2)
                       ? "python3 runtime/ref_sol.py < runtime/input.txt > runtime/ref_out.txt"
                       : "./runtime/ref_sol.out < runtime/input.txt > runtime/ref_out.txt";

    std::vector<int> test_sizes = {1000, 3000, 5000, 8000, 12000};
    auto run_ref = [&]()
    { std::system(cmd_ref); };
    auto run_stud = [&]()
    { std::system(cmd_stud); };

    for (size_t t = 0; t < test_sizes.size(); ++t)
    {
        add_result("Running Test " + std::to_string(t + 1) + " (N=" + std::to_string(test_sizes[t]) + ")...");
        ValidateSorting(run_ref, run_stud, t + 1, test_sizes[t], add_result);

        bool has_error = false;
        {
            std::lock_guard<std::mutex> lock(state->results_mutex);
            if (!state->judging_results.empty() &&
                state->judging_results.back().find("ACCEPTED") == std::string::npos)
            {
                has_error = true;
            }
        }
        if (has_error)
            break;
    }

    add_result("Judging Complete.");
    state->SetFinished(true);
    on_ui_update();
}