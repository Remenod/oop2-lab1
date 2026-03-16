#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <algorithm>
#include <random>

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

void selection_sort_ref(std::vector<int> &arr)
{
    for (size_t i = 0; i < arr.size() - 1; ++i)
    {
        size_t min_idx = i;
        for (size_t j = i + 1; j < arr.size(); ++j)
        {
            if (arr[j] < arr[min_idx])
                min_idx = j;
        }
        std::swap(arr[i], arr[min_idx]);
    }
}

void shaker_sort_ref(std::vector<int> &arr)
{
    bool swapped = true;
    int start = 0, end = arr.size() - 1;
    while (swapped)
    {
        swapped = false;
        for (int i = start; i < end; ++i)
        {
            if (arr[i] > arr[i + 1])
            {
                std::swap(arr[i], arr[i + 1]);
                swapped = true;
            }
        }
        if (!swapped)
            break;
        swapped = false;
        --end;
        for (int i = end - 1; i >= start; --i)
        {
            if (arr[i] > arr[i + 1])
            {
                std::swap(arr[i], arr[i + 1]);
                swapped = true;
            }
        }
        ++start;
    }
}

bool is_time_valid(long long t_eta, long long t_stud)
{
    long long lower_bound = std::max(0LL, t_eta / 5 - 200);
    long long upper_bound = 5 * t_eta + 200;
    return t_stud >= lower_bound && t_stud <= upper_bound;
}

void JudgeThread(ScreenInteractive *screen, int problem_idx, int lang_idx, std::string code)
{
    auto add_result = [&](std::string msg)
    {
        std::lock_guard<std::mutex> lock(results_mutex);
        judging_results.push_back(msg);
        screen->PostEvent(Event::Custom); // Оновлюємо UI
    };

    std::string ext = (lang_idx == 0) ? ".c" : ((lang_idx == 1) ? ".cpp" : ".py");
    std::string filename = "solution" + ext;
    std::ofstream out(filename);
    out << code;
    out.close();

    add_result("[System] Code saved to " + filename);

    if (lang_idx == 0 || lang_idx == 1)
    {
        add_result("[System] Compiling...");
        std::string comp_cmd = (lang_idx == 0) ? "gcc solution.c -o solution.out" : "g++ solution.cpp -o solution.out";
        int comp_res = std::system(comp_cmd.c_str());
        if (comp_res != 0)
        {
            add_result("Result: COMPILATION ERROR");
            judging_finished = true;
            screen->PostEvent(Event::Custom);
            return;
        }
        add_result("[System] Compilation Successful.");
    }

    std::vector<int> test_sizes = {1000, 5000, 10000, 20000, 30000};

    for (size_t t = 0; t < test_sizes.size(); ++t)
    {
        int N = test_sizes[t];
        add_result("Running Test " + std::to_string(t + 1) + " (N=" + std::to_string(N) + ")...");

        std::vector<int> arr(N);
        std::mt19937 rng(42 + t);
        for (int i = 0; i < N; ++i)
            arr[i] = rng() % 100000;

        std::vector<int> ref_arr = arr;

        auto start_eta = high_resolution_clock::now();
        if (problem_idx == 0)
            selection_sort_ref(ref_arr);
        else
            shaker_sort_ref(ref_arr);
        auto end_eta = high_resolution_clock::now();
        long long t_eta = duration_cast<milliseconds>(end_eta - start_eta).count();

        std::ofstream in_file("input.txt");
        in_file << N << "\n";
        for (int x : arr)
            in_file << x << " ";
        in_file.close();

        std::string run_cmd = (lang_idx == 2) ? "python3 solution.py < input.txt > output.txt"
                                              : "./solution.out < input.txt > output.txt";

        auto start_stud = high_resolution_clock::now();
        int run_res = std::system(run_cmd.c_str());
        auto end_stud = high_resolution_clock::now();
        long long t_stud = duration_cast<milliseconds>(end_stud - start_stud).count();

        if (run_res != 0)
        {
            add_result("Test " + std::to_string(t + 1) + ": RUNTIME ERROR");
            break;
        }

        std::ifstream out_file("output.txt");
        bool is_correct = true;
        for (int i = 0; i < N; ++i)
        {
            int val;
            if (!(out_file >> val) || val != ref_arr[i])
            {
                is_correct = false;
                break;
            }
        }
        out_file.close();

        if (!is_correct)
        {
            add_result("Test " + std::to_string(t + 1) + ": WRONG ANSWER");
            break;
        }
        else if (!is_time_valid(t_eta, t_stud))
        {
            add_result("Test " + std::to_string(t + 1) + ": TIME ERROR (Ref: " + std::to_string(t_eta) + "ms, Stud: " + std::to_string(t_stud) + "ms)");
            break;
        }
        else
        {
            add_result("Test " + std::to_string(t + 1) + ": ACCEPTED (Ref: " + std::to_string(t_eta) + "ms, Stud: " + std::to_string(t_stud) + "ms)");
        }
    }

    add_result("[System] Judging Complete.");
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
    auto renderer1 = Renderer(screen1, [&]
                              { return vbox({text("DMOJ Offline TUI - Select Problem") | bold | center,
                                             separator(),
                                             problem_menu->Render() | border,
                                             btn_next->Render() | center}) |
                                       border; });

    std::string code_content = "// Read N, then N integers. Sort them and print.\n#include <iostream>\nusing namespace std;\n\nint main() {\n    int n; cin >> n;\n    // your code here\n    return 0;\n}";
    InputOption input_opt;
    input_opt.multiline = true;
    auto code_input = Input(&code_content, "Type your code here...", input_opt);

    int selected_lang = 1;
    std::vector<std::string> languages = {"C", "C++", "Python"};
    auto lang_dropdown = Dropdown(&languages, &selected_lang);

    auto btn_submit = Button("Submit Code", [&]
                             {
        judging_results.clear();
        judging_finished = false;
        current_state = AppState::Judging;
        std::thread(JudgeThread, &screen, selected_problem, selected_lang, code_content).detach(); }, ButtonOption::Animated(Color::Green));

    auto btn_back = Button("Back", [&]
                           { current_state = AppState::ProblemSelection; });

    auto screen2 = Container::Vertical({code_input, lang_dropdown, btn_submit, btn_back});
    auto renderer2 = Renderer(screen2, [&]
                              { return vbox({text("Code Editor - " + problems[selected_problem]) | bold | center,
                                             separator(),
                                             code_input->Render() | flex | border,
                                             hbox({text("Language: ") | vcenter, lang_dropdown->Render()}),
                                             hbox({btn_back->Render(), filler(), btn_submit->Render()})}) |
                                       border | flex; });

    auto btn_finish = Button("Return to Menu", [&]
                             { current_state = AppState::ProblemSelection; });
    auto screen3 = Container::Vertical({btn_finish});
    auto renderer3 = Renderer(screen3, [&]
                              {
        std::vector<Element> results_elements;
        {
            std::lock_guard<std::mutex> lock(results_mutex);
            for (const auto& res : judging_results) {
                Color c = Color::White;
                if (res.find("ACCEPTED") != std::string::npos) c = Color::Green;
                if (res.find("ERROR") != std::string::npos || res.find("WRONG") != std::string::npos) c = Color::Red;
                results_elements.push_back(text(res) | color(c));
            }
        }

        auto layout = vbox({
            text("Live Judging Results") | bold | center,
            separator(),
            vbox(results_elements) | flex,
            judging_finished ? btn_finish->Render() | center : text("Judging in progress...") | blink | center
        }) | border | flex;
        
        return layout; });

    auto main_container = Container::Tab({renderer1, renderer2, renderer3}, (int *)&current_state);

    screen.Loop(main_container);

    return 0;
}