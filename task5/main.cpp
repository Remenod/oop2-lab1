#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Component CreateHelloWorldWindow(bool *show_signal)
{
    struct WindowState
    {
        int x = 10;
        int y = 5;
        int width = 40;
        int height = 10;
    };
    static auto state = std::make_shared<WindowState>();

    auto close_btn = Button("Close", [show_signal]
                            { *show_signal = false; }, ButtonOption::Animated());

    auto content = Renderer(close_btn, [close_btn]
                            { return vbox({text("Hello World") | bold | center | flex,
                                           close_btn->Render() | center}); });

    return Window({
        .inner = content,
        .title = "Hello who",
        .left = &state->x,
        .top = &state->y,
        .width = &state->width,
        .height = &state->height,
    });
}

int main()
{
    auto screen = ScreenInteractive::Fullscreen();
    Color current_bg_color = Color::Default;

    bool is_window_open = false;
    Component my_window = CreateHelloWorldWindow(&is_window_open);

    bool cb1 = false, cb2 = false, cb3 = false;
    auto transparency_ch = Checkbox("SuperMegaButton absorbs Transparency", &cb1);
    auto background_ch = Checkbox("SuperMegaButton absorbs Background Color", &cb2);
    auto hello_world_ch = Checkbox("SuperMegaButton absorbs Hello World", &cb3);

    auto transparency_b = Button("Transparency", [] {});
    auto background_b = Button("Background Color", [] {});
    auto hello_world_b = Button("Hello World", [&]
                                { 
                                    is_window_open = true; 
                                    my_window->TakeFocus(); });
    auto wide_button = Button("SuperMegaButton", [] {});

    auto buttons_row = Renderer(
        Container::Horizontal({transparency_b, background_b, hello_world_b}), [&]
        { return hbox({
                     transparency_b->Render() | flex,
                     background_b->Render() | flex,
                     hello_world_b->Render() | flex,
                 }) |
                 border; });

    auto checkboxes_col = Renderer(
        Container::Vertical({transparency_ch, background_ch, hello_world_ch}), [&]
        { return vbox({
              transparency_ch->Render(),
              background_ch->Render(),
              hello_world_ch->Render(),
          }); });

    auto wide_button_renderer = Renderer(
        Container::Horizontal({wide_button}), [&]
        { return hbox({
                     wide_button->Render() | flex,
                 }) |
                 border; });

    auto main_content = Container::Vertical({
        buttons_row,
        wide_button_renderer,
        checkboxes_col,
    });

    auto main_stack = Container::Stacked({
        main_content,
        my_window,
    });

    auto final_component = Renderer(main_stack, [&]
                                    {
        Element base = main_content->Render();

        if (!is_window_open) {
            return base;
        }

        return dbox({
            base,
            my_window->Render() | color(Color::White)
        }); });

    screen.Loop(final_component);

    return 0;
}