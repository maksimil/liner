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

    std::future<void> begin(const char *windowname, const uint32_t &width,
                            const uint32_t &height);
    void end();

    void render();
    void draw(const Line &line);

    static Renderer &get();
};