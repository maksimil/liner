#include "renderer.h"
#include "../utils/profiler.h"
#include <SFML/Graphics.hpp>

using namespace sf;

Renderer &Renderer::get()
{
    static Renderer instance;
    return instance;
}

void Renderer::begin(const char *windowname)
{
    TSCOPEID("intialize window", 20);
    window = new RenderWindow(VideoMode(600, 600), windowname);
    tsc20.stop();
    window->setActive(false);
}

void Renderer::render()
{
    rendertaskfuture = std::async(
        std::launch::async,
        [](std::vector<Line> lines) {
            Renderer &renderer = Renderer::get();

            std::lock_guard lk(renderer.windowmutex);
            RenderWindow &window = *renderer.window;

            TSCOPE("render");
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear();

            for (size_t i = 0; i < lines.size(); i++)
            {
                const Line &line = lines[i];

                window.draw(&line[0], line.size(), LineStrip);
            }

            window.display();

            window.setActive(false);
        },
        lines);

    lines.clear();
}

void Renderer::end()
{
    window->close();
    delete window;
}

void Renderer::draw(const Line &line)
{
    lines.push_back(line);
}