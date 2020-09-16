#include "renderer.h"
#include "../utils/profiler.h"
#include <SFML/Graphics.hpp>

Renderer &Renderer::get()
{
  static Renderer instance;
  return instance;
}

void Renderer::begin(const char *windowname, const MSTYPE &period)
{
  updated = false;
  running = true;
  renderthread = std::thread(
      [](const char *winname, const MSTYPE updateperiod) {
        using namespace sf;

        Renderer &renderer = Renderer::get();

        const uint32_t width = 600;

        Profiler &profiler = Profiler::get();

        TSCOPEID("intialize window", 20);
        RenderWindow window(VideoMode(width, width), winname);
        tsc20.stop();

        auto lastupdate = NOW;

        while (renderer.running && window.isOpen())
        {
          WAIT_UNTIL(lastupdate, updateperiod)

          if (renderer.updated)
          {
            TSCOPE("render");
            Event event;
            while (window.pollEvent(event))
            {
              if (event.type == Event::Closed)
                window.close();
            }

            window.clear();

            std::vector<Line> lines = renderer.lines;

            for (size_t i = 0; i < lines.size(); i++)
            {
              const Line &line = lines[i];

              window.draw(&line[0], line.size(), LineStrip);
            }

            window.display();

            renderer.lines.clear();
            renderer.updated = false;
          }
        }
      },
      windowname, period);
}

void Renderer::update()
{
  updated = true;
}

void Renderer::end()
{
  running = false;
  renderthread.join();
}

void Renderer::draw(const Line &line)
{
  lines.push_back(line);
}