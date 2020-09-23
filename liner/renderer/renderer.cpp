#include "renderer.h"
#include "../utils/profiler.h"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

Renderer &Renderer::get()
{
    static Renderer instance;
    return instance;
}

std::future<void> Renderer::begin(const char *windowname, const uint32_t &width,
                                  const uint32_t &height)
{
    return std::async(
        std::launch::async,
        [](const char *windowname, const uint32_t width,
           const uint32_t height) {
            TSCOPE("initialize window");
            RenderWindow *&window = Renderer::get().window;
            window = new RenderWindow(VideoMode(width, height), windowname);
            window->setActive(false);
        },
        windowname, width, height);
}

void Renderer::render()
{
    if (rendertaskfuture.valid())
        rendertaskfuture.wait();

    if (window->isOpen())
        rendertaskfuture = std::async(
            std::launch::async,
            [](std::vector<Line> lines) {
                TSCOPE("render");
                Renderer &renderer = Renderer::get();

                std::lock_guard lk(renderer.windowmutex);
                RenderWindow &window = *renderer.window;

                renderer.events.clear();
                sf::Event event;
                while (window.pollEvent(event))
                {
                    switch (event.type)
                    {
                    case sf::Event::KeyPressed:
                    case sf::Event::KeyReleased:
                        renderer.events.push_back(event);
                        break;
                    case sf::Event::Closed:
                        window.close();
                        break;
                    }
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