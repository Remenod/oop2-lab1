#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <functional>
#include <random>
#include "CodeGenerators.hpp"

using namespace ftxui;
using namespace std::chrono;

enum class AppState
{
    ProblemSelection,
    CodeInput,
    Judging
};

std::vector<std::string> judging_results;
std::mutex results_mutex;
bool judging_finished = false;

template <typename Func>
long long MeasureTime(Func action)
{
    auto start = high_resolution_clock::now();
    action();
    auto end = high_resolution_clock::now();
    return duration_cast<milliseconds>(end - start).count();
}

void ValidateSorting(
    std::function<void()> ref_action,
    std::function<void()> stud_action,
    int test_num,
    int N,
    const std::function<void(std::string)> &ui_cb)
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

    long long lower_bound = std::max(0LL, t_eta / 5 - 200);
    long long upper_bound = 5 * t_eta + 200;
    std::string time_str =
        " (Ref: " + std::to_string(t_eta) + "ms, Stud: " + std::to_string(t_stud) + "ms)";

    if (t_stud < lower_bound)
    {
        ui_cb("Test " + std::to_string(test_num) + ": TIME ERROR (Too Fast)" + time_str);
    }
    else if (t_stud > upper_bound)
    {
        ui_cb("Test " + std::to_string(test_num) + ": TIME ERROR (Too Slow)" + time_str);
    }
    else
    {
        ui_cb("Test " + std::to_string(test_num) + ": ACCEPTED" + time_str);
    }
}

void JudgeThread(ScreenInteractive *screen, int problem_idx, int lang_idx, std::string code)
{
    auto add_result = [&](std::string msg)
    {
        std::lock_guard<std::mutex> lock(results_mutex);
        judging_results.push_back(msg);
        screen->PostEvent(Event::Custom);
    };

    std::filesystem::create_directories("runtime");

    std::string ext = (lang_idx == 0)
                          ? ".c"
                          : ((lang_idx == 1)
                                 ? ".cpp"
                                 : ".py");
    std::string stud_filename = "./runtime/stud_sol" + ext;
    std::string ref_filename = "./runtime/ref_sol" + ext;

    std::ofstream out_stud(stud_filename);
    out_stud << code;
    out_stud.close();

    std::ofstream out_ref(ref_filename);
    out_ref << GetReferenceCode(static_cast<SolveProblem>(problem_idx),
                                static_cast<SolveLanguage>(lang_idx));
    out_ref.close();

    if (lang_idx == 0 || lang_idx == 1)
    {
        std::string compiler = (lang_idx == 0) ? "gcc" : "g++";
        if (std::system((compiler + " " + stud_filename + " -o runtime/stud_sol.out").c_str()) != 0)
        {
            add_result("Result: COMPILATION ERROR");
            judging_finished = true;
            screen->PostEvent(Event::Custom);
            return;
        }
        std::system((compiler + " " + ref_filename + " -o runtime/ref_sol.out").c_str());
    }

    auto cmd_stud = (lang_idx == 2)
                        ? "python3 runtime/stud_sol.py < runtime/input.txt > runtime/stud_out.txt"
                        : "./runtime/stud_sol.out < runtime/input.txt > runtime/stud_out.txt";
    auto cmd_ref = (lang_idx == 2)
                       ? "python3 runtime/ref_sol.py < runtime/input.txt > runtime/ref_out.txt"
                       : "./runtime/ref_sol.out < runtime/input.txt > runtime/ref_out.txt";

    std::vector<int> test_sizes = {1000, 3000, 5000, 8000, 12000};

    std::function<void()> run_ref = [&]()
    { std::system(cmd_ref); };
    std::function<void()> run_stud = [&]()
    { std::system(cmd_stud); };

    for (size_t t = 0; t < test_sizes.size(); ++t)
    {
        add_result("Running Test " + std::to_string(t + 1) +
                   " (N=" + std::to_string(test_sizes[t]) + ")...");
        ValidateSorting(run_ref, run_stud, t + 1, test_sizes[t], add_result);

        bool has_error = false;
        {
            std::lock_guard<std::mutex> lock(results_mutex);
            if (judging_results.back().find("ACCEPTED") == std::string::npos)
                has_error = true;
        }
        if (has_error)
            break;
    }

    add_result("Judging Complete.");
    judging_finished = true;
    screen->PostEvent(Event::Custom);
}

int main()
{
    auto screen = ScreenInteractive::Fullscreen();

    AppState current_state = AppState::ProblemSelection;

    int selected_problem = 0;
    std::vector<std::string> problems = {"Selection Sort", "Shaker Sort"};
    auto problem_menu = Radiobox(&problems, &selected_problem);
    auto btn_next = Button("Proceed to Editor", [&]
                           { current_state = AppState::CodeInput; }, ButtonOption::Animated());

    auto screen1 = Container::Vertical({problem_menu, btn_next});
    auto renderer1 =
        Renderer(screen1, [&]
                 { return vbox({text("DMOJ Offline TUI - Select Problem") | bold | center,
                                separator(),
                                problem_menu->Render() | border,
                                btn_next->Render() | center}) |
                          border; });

    std::string code_content = "// Read N, then N integers. Sort them and print.\n"
                               "#include <iostream>\nusing namespace std;\n\n"
                               "int main() {\n"
                               "    int n; cin >> n;\n"
                               " // your code here\n"
                               "    return 0;\n"
                               "}";
    InputOption input_opt;
    input_opt.multiline = true;
    auto code_input = Input(&code_content, "Type your code here...", input_opt);

    int selected_lang = 1;
    std::vector<std::string> languages = {"C", "C++", "Python"};
    auto lang_dropdown = Dropdown(&languages, &selected_lang);

    auto btn_submit =
        Button("Submit Code", [&]
               {
                    judging_results.clear();
                    judging_finished = false;
                    current_state = AppState::Judging;
                    std::thread(
                            JudgeThread, 
                            &screen, 
                            selected_problem, 
                            selected_lang, 
                            code_content).detach(); }, ButtonOption::Animated(Color::Green));

    auto btn_back = Button("Back", [&]
                           { current_state = AppState::ProblemSelection; });

    auto screen2 = Container::Vertical({code_input, lang_dropdown, btn_submit, btn_back});
    auto renderer2 =
        Renderer(screen2, [&]
                 { return vbox(
                              {text("Code Editor - " + problems[selected_problem]) | bold | center,
                               separator(),
                               code_input->Render() | flex | border,
                               hbox({text("Language: ") | vcenter, lang_dropdown->Render()}),
                               hbox({btn_back->Render(), filler(), btn_submit->Render()})}) |
                          border | flex; });

    auto btn_finish = Button("Return to Menu", [&]
                             { current_state = AppState::ProblemSelection; });
    auto screen3 = Container::Vertical({btn_finish});
    auto renderer3 =
        Renderer(screen3, [&]
                 {
                    std::vector<Element> results_elements;
                    {
                        std::lock_guard<std::mutex> lock(results_mutex);
                        for (const auto& res : judging_results) {
                            Color c = Color::White;
                            if (res.find("ACCEPTED") != std::string::npos)
                            {
                                c = Color::Green;
                            }
                            if (res.find("ERROR") != std::string::npos || 
                                res.find("WRONG") != std::string::npos)
                            {
                                c = Color::Red;
                            }
                            results_elements.push_back(text(res) | color(c));
                        }
                    }

                    return vbox(
                        {
                            text("Live Judging Results") | bold | center,
                            separator(),
                            vbox(results_elements) | flex,
                            judging_finished 
                                ? btn_finish->Render() | center 
                                : text("Judging in progress...") | blink | center
                        }) | border | flex; });

    auto main_container =
        Container::Tab(
            {
                renderer1,
                renderer2,
                renderer3,
            },
            (int *)&current_state);

    screen.Loop(main_container);

    return 0;
}