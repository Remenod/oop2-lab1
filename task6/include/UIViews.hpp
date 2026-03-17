#pragma once
#include <ftxui/component/component.hpp>
#include "AppState.hpp"
#include "JudgeEngine.hpp"

// Функції, які будують окремі екрани нашого додатку
ftxui::Component BuildProblemSelectionView(AppState &state);
ftxui::Component BuildCodeEditorView(AppState &state, JudgeEngine &engine);
ftxui::Component BuildResultsView(AppState &state);