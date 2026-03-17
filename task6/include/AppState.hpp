#pragma once
#include <string>
#include <vector>
#include <mutex>

enum class AppScreen
{
    ProblemSelection = 0,
    CodeInput,
    JudgingResults
};

struct AppState
{
    AppScreen current_screen = AppScreen::ProblemSelection;

    const std::vector<std::string> problems = {"Selection Sort", "Shaker Sort"};
    const std::vector<std::string> languages = {"C", "C++", "Python"};

    int selected_problem = 0;
    int selected_lang = 1;
    std::string code_content =
        "// Read N, then N integers. Sort them and print.\n"
        "#include <iostream>\nusing namespace std;\n\n"
        "int main() {\n"
        "    int n; cin >> n;\n"
        "    // your code here\n"
        "    return 0;\n"
        "}";

    std::vector<std::string> judging_results;
    bool judging_finished = false;
    std::mutex results_mutex;

    void AddResult(const std::string &msg)
    {
        std::lock_guard<std::mutex> lock(results_mutex);
        judging_results.push_back(msg);
    }

    void ClearResults()
    {
        std::lock_guard<std::mutex> lock(results_mutex);
        judging_results.clear();
        judging_finished = false;
    }

    void SetFinished(bool finished)
    {
        std::lock_guard<std::mutex> lock(results_mutex);
        judging_finished = finished;
    }
};