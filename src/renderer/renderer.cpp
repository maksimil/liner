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
         const MSTYPE updateperiod) {
        using namespace sf;

        Profiler &profiler = Profiler::get();

        TSCOPEID("intialize window", 20);
        RenderWindow window(VideoMode(600, 600), winname);
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
            // render
            window.display();

            *updated = false;
          }
        }
      },
      windowname, &running, &updated, period);
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