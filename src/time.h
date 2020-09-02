#include <chrono>

#define NOW    std::chrono::system_clock::now()
#define DURMS  std::chrono::duration_cast<std::chrono::milliseconds>
#define MSTYPE std::chrono::milliseconds