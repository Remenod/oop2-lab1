#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "../include/AppState.hpp"
#include "../include/JudgeEngine.hpp"
#include "../include/UIViews.hpp"

using namespace ftxui;

int main()
{
    auto screen = ScreenInteractive::Fullscreen();
    AppState state;

    JudgeEngine judge([&screen]()
                      { screen.PostEvent(Event::Custom); });

    auto screen1 = BuildProblemSelectionView(state);
    auto screen2 = BuildCodeEditorView(state, judge);
    auto screen3 = BuildResultsView(state);

    auto main_container = Container::Tab(
        {screen1, screen2, screen3},
        reinterpret_cast<int *>(&state.current_screen));

    screen.Loop(main_container);

    judge.StopAll();

    return 0;
}