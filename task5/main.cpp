#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <csignal>
#include <functional>
#include <iostream>

using namespace ftxui;

ftxui::ScreenInteractive *g_screen = nullptr;

enum Action
{
    Transparency,
    Background,
    Hello
};

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
        g_screen->ExitLoopClosure()();
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

    auto do_transparency = []
    {
        static float opacity = 1.0f;
        if (opacity > 0.15f)
            opacity -= 0.2f;
        else
            opacity = 1.0f;

        char buf[64];
        std::snprintf(buf, sizeof(buf), "kitty @ set-background-opacity %.1f &", opacity);
        std::system(buf);
    };

    auto do_background = [&]
    {
        static int color_iter = 0;
        color_iter++;
        if (color_iter > 4)
        {
            current_bg_color = Color::Default;
            color_iter = 0;
        }
        else
        {
            current_bg_color = Color::RGB(rand() % 255, rand() % 255, rand() % 255);
        }
    };

    auto do_hello = [&]
    {
        is_hello_window_open = true;
        hello_window->TakeFocus();
    };

    std::vector<Action> cb_order;
    auto add_order = [&](Action a)
    {
        if (std::find(cb_order.begin(), cb_order.end(), a) == cb_order.end())
            cb_order.push_back(a);
    };
    auto remove_order = [&](Action a)
    {
        cb_order.erase(std::remove(cb_order.begin(), cb_order.end(), a), cb_order.end());
    };

    bool cb1 = false, cb2 = false, cb3 = false;
    auto transparency_cb = Checkbox("SuperMegaButton absorbs Transparency", &cb1);
    transparency_cb |=
        CatchEvent([&](Event e)
                   {
                        if(e == Event::Return){
                            if(cb1) add_order(Action::Transparency);
                            else remove_order(Action::Transparency);
                        }
                        return false; });

    auto background_cb = Checkbox("SuperMegaButton absorbs Background Color", &cb2);
    background_cb |=
        CatchEvent([&](Event e)
                   {
                        if(e == Event::Return){
                            if(cb2) add_order(Action::Background);
                            else remove_order(Action::Background);
                        }
                        return false; });

    auto hello_world_cb = Checkbox("SuperMegaButton absorbs Hello World", &cb3);
    hello_world_cb |=
        CatchEvent([&](Event e)
                   {
                        if(e == Event::Return){
                            if(cb3) add_order(Action::Hello);
                            else remove_order(Action::Hello);
                        }
                        return false; });

    auto transparency_b = Button("Transparency", do_transparency);
    auto background_b = Button("Background Color", do_background);
    auto hello_world_b = Button("Hello World", do_hello);

    auto sbutton =
        Button("SuperMegaButton", [&]
               {
                    is_sbutton_window_open = true; 
                    sbutton_window->TakeFocus();
                    for(auto a : cb_order)
                    {
                        switch(a)
                        {
                            case Transparency: do_transparency(); break;
                            case Background:   do_background();   break;
                            case Hello:        do_hello();        break;
                        }
                    } });

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

    auto main_content = Container::Vertical({
        buttons_row,
        sbutton_renderer,
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
                Element base = main_content->Render() | bgcolor(current_bg_color);

                std::vector<Element> layers = {base};

                if (is_hello_window_open)
                    layers.push_back(hello_window->Render() | color(Color::White));

                if (is_sbutton_window_open)
                    layers.push_back(sbutton_window->Render() | color(Color::White));

                return dbox(layers); });

    screen.Loop(final_component);

    std::system("kitty @ set-background-opacity 1");

    return 0;
}