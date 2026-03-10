#include <notcurses/notcurses.h>
#include <vector>
#include <functional>
#include <string>

struct Widget
{
    int x;
    int y;
    bool focused = false;

    virtual void draw(ncplane *p) = 0;
    virtual void key(ncinput) {}
    virtual void mouse(ncinput) {}
    virtual bool focusable() { return true; }
    virtual ~Widget() {}
};

struct Checkbox : Widget
{
    bool checked = false;
    std::string label;

    Checkbox(int x, int y, std::string label = "")
    {
        this->x = x;
        this->y = y;
        this->label = label;
    }

    void draw(ncplane *p) override
    {
        char c = checked ? 'x' : ' ';
        if (focused)
            ncplane_printf_yx(p, y, x, "> [%c] %s", c, label.c_str());
        else
            ncplane_printf_yx(p, y, x, "  [%c] %s", c, label.c_str());
    }

    void key(ncinput in) override
    {
        if (in.id == ' ')
            checked = !checked;
    }

    void mouse(ncinput in) override
    {
        if (in.evtype == NCTYPE_RELEASE)
            checked = !checked;
    }
};

struct Button : Widget
{
    std::string text;
    std::function<void()> onclick;

    Button(int x, int y, std::string text, std::function<void()> fn)
    {
        this->x = x;
        this->y = y;
        this->text = text;
        onclick = fn;
    }

    void draw(ncplane *p) override
    {
        if (focused)
            ncplane_printf_yx(p, y, x, "< %s >", text.c_str());
        else
            ncplane_printf_yx(p, y, x, "  %s  ", text.c_str());
    }

    void key(ncinput in) override
    {
        if (in.id == NCKEY_ENTER)
            onclick();
    }

    void mouse(ncinput in) override
    {
        if (in.evtype == NCTYPE_RELEASE)
            onclick();
    }
};

struct UI
{
    std::vector<Widget *> widgets;
    int focus = 0;

    void add(Widget *w)
    {
        widgets.push_back(w);
    }

    void draw(ncplane *p)
    {
        for (auto w : widgets)
            w->draw(p);
    }

    void focus_next()
    {
        if (widgets.empty())
            return;

        widgets[focus]->focused = false;

        do
        {
            focus = (focus + 1) % widgets.size();
        } while (!widgets[focus]->focusable());

        widgets[focus]->focused = true;
    }

    void key(ncinput in)
    {
        if (in.evtype == NCTYPE_RELEASE)
        {
            if (in.id == '\t')
            {
                focus_next();
                return;
            }

            widgets[focus]->key(in);
        }
    }

    void mouse(ncinput in)
    {
        for (auto w : widgets)
        {
            if (in.y == w->y)
                w->mouse(in);
        }
    }
};

int main()
{
    notcurses_options opts{};
    notcurses *nc = notcurses_init(&opts, nullptr);
    notcurses_mice_enable(nc, NCMICE_BUTTON_EVENT);

    ncplane *stdplane = notcurses_stdplane(nc);

    UI ui;

    Checkbox transparency_cb(2, 4, "SuperMegaButton absorbs Transparency");
    Checkbox bg_color_cb(2, 5, "SuperMegaButton absorbs Background color");
    Checkbox hello_world_cb(2, 6, "SuperMegaButton absorbs Hello World");

    bool quit = false;

    Button transparency_button(2, 2, "Transparency", [&] {});
    Button bg_color_button(19, 2, "Background Color", [&] {});
    Button hello_world_button(40, 2, "Hello World", [&] {});
    Button superButton(19, 3, "SuperMegaButton", [&] {});

    ui.add(&transparency_button);
    ui.add(&bg_color_button);
    ui.add(&hello_world_button);
    ui.add(&superButton);
    ui.add(&transparency_cb);
    ui.add(&bg_color_cb);
    ui.add(&hello_world_cb);

    ui.widgets[0]->focused = true;

    while (!quit)
    {
        ncplane_erase(stdplane);

        ui.draw(stdplane);

        notcurses_render(nc);

        ncinput in;
        timespec ts{0, 0};

        uint32_t key = notcurses_get(nc, &ts, &in);

        if (key == 0)
            continue;

        if (nckey_mouse_p(key) && in.evtype == NCTYPE_RELEASE)
        {
            ui.mouse(in);
        }
        else if (!nckey_mouse_p(key))
        {
            ui.key(in);
        }
    }

    notcurses_stop(nc);
}