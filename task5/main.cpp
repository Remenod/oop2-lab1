#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <csignal>
#include <functional>

using namespace ftxui;

ftxui::ScreenInteractive *g_screen = nullptr;

Component CreateTextWindow(bool *show_signal, std::string message)
{
    int x = 10;
    int y = 5;
    int width = 40;
    int height = 10;

    auto close_btn = Button("Close", [show_signal]
                            { *show_signal = false; }, ButtonOption::Animated());

    auto content = Renderer(close_btn, [close_btn, message]
                            { return vbox({paragraph(message) | bold | center | flex,
                                           close_btn->Render() | center}); });

    return Window({
        .inner = content,
        .title = "",
        .left = x,
        .top = y,
        .width = width,
        .height = height,
    });
}

void SignalHandler(int signal)
{
    if (g_screen)
    {
        g_screen->ExitLoopClosure()();
    }
}

void set_kitty_bg(int r, int g, int b)
{
    char buf[128];
    std::snprintf(buf, sizeof(buf),
                  "kitty @ set-colors background=#%02x%02x%02x &", r, g, b);
    std::system(buf);
}

void set_kitty_opacity(float opacity)
{
    char buf[64];
    std::snprintf(buf, sizeof(buf),
                  "kitty @ set-background-opacity %.2f &", opacity);
    std::system(buf);
}

int main()
{
    std::signal(SIGINT, SignalHandler);

    auto screen = ScreenInteractive::Fullscreen();
    g_screen = &screen;

    Color current_bg_color = Color::Default;

    bool is_hello_window_open = false;
    Component hello_window = CreateTextWindow(&is_hello_window_open, "Hello World!!!!");
    bool is_sbutton_window_open = false;
    Component sbutton_window =
        CreateTextWindow(&is_sbutton_window_open,
                         "        I am a SuperMegaButton, \n"
                         "and you cannot take that away from me!");

    bool cb1 = false, cb2 = false, cb3 = false;
    auto transparency_cb = Checkbox("SuperMegaButton absorbs Transparency", &cb1);
    auto background_cb = Checkbox("SuperMegaButton absorbs Background Color", &cb2);
    auto hello_world_cb = Checkbox("SuperMegaButton absorbs Hello World", &cb3);

    auto do_transparency = [&]
    {
        static float opacity = 1.0f;
        if (opacity > 0.15f)
            opacity -= 0.2f;
        else
            opacity = 1.0f;

        set_kitty_opacity(opacity);
    };

    auto do_background = [&]
    {
        static int color_iter = 0;
        color_iter++;
        if (color_iter > 4)
        {
            set_kitty_bg(0x11, 0x11, 0x11);
            color_iter = 0;
        }
        else
        {
            int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;
            set_kitty_bg(r, g, b);
        }
    };

    auto do_hello = [&]
    {
        is_hello_window_open = true;
        hello_window->TakeFocus();
    };

    auto transparency_b = Button("Transparency", do_transparency);
    auto background_b = Button("Background Color", do_background);
    auto hello_world_b = Button("Hello World", do_hello);

    auto sbutton =
        Button("SuperMegaButton", [&]
               {
                   if (cb1) do_transparency();
                   if (cb2) do_background();
                   if (cb3) do_hello();
                   is_sbutton_window_open = true; 
                   sbutton_window->TakeFocus(); });

    auto roll_button =
        Button("Roll Button", [&]
               { std::system("bash ../spin.sh &"); });

    auto buttons_row = Renderer(
        Container::Horizontal({transparency_b, background_b, hello_world_b}), [&]
        { return hbox({
                     transparency_b->Render() | flex,
                     background_b->Render() | flex,
                     hello_world_b->Render() | flex,
                 }) |
                 border; });

    auto checkboxes_col = Renderer(
        Container::Vertical({transparency_cb, background_cb, hello_world_cb}), [&]
        { return vbox({
              transparency_cb->Render(),
              background_cb->Render(),
              hello_world_cb->Render(),
          }); });

    auto sbutton_renderer = Renderer(
        Container::Horizontal({sbutton}), [&]
        { return hbox({
                     sbutton->Render() | flex,
                 }) |
                 border; });

    auto roll_button_renderer = Renderer(
        Container::Horizontal({roll_button}), [&]
        { return hbox({
                     roll_button->Render() | flex,
                 }) |
                 border; });

    auto main_content = Container::Vertical({
        buttons_row,
        sbutton_renderer,
        roll_button_renderer,
        checkboxes_col,
    });

    auto main_stack = Container::Stacked({
        main_content,
        hello_window,
        sbutton_window,
    });

    auto final_component =
        Renderer(main_stack, [&]
                 {
                Element base = main_content->Render();

                std::vector<Element> layers = {base};

                if (is_hello_window_open)
                    layers.push_back(hello_window->Render() | color(Color::White));

                if (is_sbutton_window_open)
                    layers.push_back(sbutton_window->Render() | color(Color::White));

                return dbox(layers); });

    screen.Loop(final_component);

    set_kitty_opacity(1.0f);
    set_kitty_bg(0x11, 0x11, 0x11);

    return 0;
}