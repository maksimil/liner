#pragma once

#include <chrono>

#define NOW      std::chrono::high_resolution_clock::now()
#define DURMS    std::chrono::duration_cast<std::chrono::milliseconds>
#define MSTYPE   std::chrono::milliseconds
#define TIMETYPE std::chrono::high_resolution_clock::time_point

#define WAIT_UNTIL(lastupdate, period)                                         \
  auto now = NOW;                                                              \
  auto duration = DURMS(now - lastupdate);                                     \
  if (duration < period)                                                       \
  {                                                                            \
    std::this_thread::sleep_for(period - duration);                            \
  }                                                                            \
  lastupdate = NOW;