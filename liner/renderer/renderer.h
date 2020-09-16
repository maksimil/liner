#pragma once

#include "../utils/time.h"
#include <SFML/Graphics.hpp>
#include <future>
#include <thread>
#include <vector>

using Line = std::vector<sf::Vertex>;

struct Renderer
{
  std::vector<Line> lines = {};

  std::thread renderthread;

  bool running = false;
  bool updated = true;

  void begin(const char *windowname, const MSTYPE &period);
  void end();

  void update();
  void draw(const Line &line);

  static Renderer &get();
};