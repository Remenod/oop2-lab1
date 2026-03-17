#pragma once
#include <ftxui/component/component.hpp>
#include "AppState.hpp"
#include "JudgeEngine.hpp"

ftxui::Component BuildProblemSelectionView(AppState &state);
ftxui::Component BuildCodeEditorView(AppState &state, JudgeEngine &engine);
ftxui::Component BuildResultsView(AppState &state);