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
      [](const char *winname, bool *running, bool *updated,
         const MSTYPE updateperiod, std::vector<Line> *lines) {
        using namespace sf;

        const uint32_t width = 600;

        Profiler &profiler = Profiler::get();

        TSCOPEID("intialize window", 20);
        RenderWindow window(VideoMode(width, width), winname);
        tsc20.stop();

        auto lastupdate = NOW;

        while (*running && window.isOpen())
        {
          WAIT_UNTIL(lastupdate, updateperiod)

          if (*updated)
          {
            TSCOPE("render");
            Event event;
            while (window.pollEvent(event))
            {
              if (event.type == Event::Closed)
                window.close();
            }

            window.clear();

            for (size_t i = 0; i < lines->size(); i++)
            {
              const Line &line = lines->at(i);
              window.draw(&line[0], line.size(), Lines);
            }

            window.display();

            *updated = false;
            lines->clear();
          }
        }
      },
      windowname, &running, &updated, period, &lines);
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