#include "console.h"
#include <iostream>

Console &Console::get()
{
  static Console instance;
  return instance;
}

void Console::log(const std::string &message)
{
  futs.push_back(std::async(
      std::launch::async, [](std::string msg) { std::cout << msg << "\n"; },
      message));
}