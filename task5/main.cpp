#include <gtkmm.h>

int main()
{
    auto app = Gtk::Application::create("org.example.gui");

    app->signal_activate().connect(
        [&]()
        {
            auto window = new Gtk::Window();
            window->set_default_size(400, 600);
            window->set_title("GTKmm 4");

            auto button = new Gtk::Button("Hello World");
            button->signal_clicked().connect(
                [window]()
                {
                    auto dialog = Gtk::AlertDialog::create("Hello World!");
                    dialog->show(*window);
                });

            window->set_child(*button);
            window->present();
        });

    app->run();
    while (true)
    {
        /* code */
    }

    return 0;
}
