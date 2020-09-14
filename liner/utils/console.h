#include <future>
#include <vector>

#define LOG Console::get().log

struct Console
{
  std::vector<std::future<void>> futs;

  void log(const std::string &message);

  static Console &get();
};