#include "../include/UIViews.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Component BuildProblemSelectionView(AppState &state)
{
    auto problem_menu = Radiobox(&state.problems, &state.selected_problem);
    auto btn_next = Button(
        "Proceed to Editor",
        [&state]
        { state.current_screen = AppScreen::CodeInput; },
        ButtonOption::Animated());

    auto container = Container::Vertical({problem_menu, btn_next});

    return Renderer(
        container,
        [=, &state]
        {
            return vbox(
                       {
                           text("Offline Judgment - Select Problem") //
                               | bold                                //
                               | center,

                           separator(),

                           problem_menu->Render() //
                               | border,

                           btn_next->Render() //
                               | center       //
                       })                     //
                   | border;
        });
}

Component BuildCodeEditorView(AppState &state, JudgeEngine &engine)
{
    InputOption input_opt;
    input_opt.multiline = true;

    input_opt.transform = [](InputState state)
    {
        if (state.focused)
            return state.element //
                   | color(Color::White);
        return state.element;
    };

    auto code_input = Input(&state.code_content, "Type your code here...", input_opt);

    auto btn_clear_code = Button(
        "Clear Code",
        [&state]
        { state.code_content = ""; });

    auto lang_dropdown = Dropdown(&state.languages, &state.selected_lang);

    auto btn_submit = Button(
        "Submit Code",
        [&state, &engine]
        {
            state.current_screen = AppScreen::JudgingResults;
            engine.RunJudgingAsync(state);
        },
        ButtonOption::Animated(Color::Green));

    auto btn_back = Button(
        "Back",
        [&state]
        { state.current_screen = AppScreen::ProblemSelection; });

    auto container = Container::Vertical(
        {
            code_input,
            btn_clear_code,
            lang_dropdown,
            btn_submit,
            btn_back,
        });

    return Renderer(
        container,
        [=, &state]
        {
            return vbox(
                       {
                           text("Code Editor - " + state.problems[state.selected_problem]) //
                               | bold                                                      //
                               | center,

                           separator(),

                           code_input->Render() //
                               | flex           //
                               | border,

                           hbox({btn_clear_code->Render()}),

                           hbox(
                               {
                                   text("Language: ") //
                                       | vcenter,
                                   lang_dropdown->Render() //
                               }),

                           hbox(
                               {
                                   btn_back->Render(),  //
                                   filler(),            //
                                   btn_submit->Render() //
                               })                       //
                       })                               //
                   | border                             //
                   | flex;
        });
}

Component BuildResultsView(AppState &state)
{
    auto btn_finish = Button(
        "Return to Menu",
        [&state]
        { state.current_screen = AppScreen::ProblemSelection; });

    auto container = Container::Vertical({btn_finish});

    return Renderer(
        container,
        [=, &state]
        {
            std::vector<Element> results_elements;
            bool is_finished = false;

            {
                std::lock_guard<std::mutex> lock(state.results_mutex);
                is_finished = state.judging_finished;
                for (const auto &res : state.judging_results)
                {
                    Color c = Color::White;
                    if (res.find("ACCEPTED") != std::string::npos)
                        c = Color::Green;
                    if (res.find("ERROR") != std::string::npos ||
                        res.find("WRONG") != std::string::npos)
                        c = Color::Red;

                    results_elements.push_back(text(res) | color(c));
                }
            }

            return vbox(
                       {
                           text("Live Judging Results") //
                               | bold                   //
                               | center,

                           separator(),

                           vbox(results_elements) //
                               | flex,

                           is_finished
                               ? btn_finish->Render() //
                                     | center
                               : text("Judging in progress...") //
                                     | blink                    //
                                     | center                   //
                       })                                       //
                   | border                                     //
                   | flex;
        });
}