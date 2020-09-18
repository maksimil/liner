#pragma once

#include <SFML/Graphics.hpp>
#include <future>
#include <thread>
#include <vector>

using Line = std::vector<sf::Vertex>;

struct Renderer
{
    std::vector<Line> lines = {};

    sf::RenderWindow *window;

    std::mutex windowmutex;

    std::future<void> rendertaskfuture;

    void begin(const char *windowname);
    void end();

    void render();
    void draw(const Line &line);

    static Renderer &get();
};