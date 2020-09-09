#include "../utils/time.h"
#include <thread>

struct Renderer
{
  std::thread renderthread;

  bool running;
  bool updated;

  void begin(const char *windowname, const MSTYPE &period);
  void end();

  void update();

  static Renderer &get();
};