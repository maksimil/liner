#include <fstream>
#include <future>
#include <string>
#include <vector>

#define TSCOPE(name) TimeScope name(#name)

struct ProfileResult
{
  int64_t start, end;
  const char *name;
  uint32_t tid;
};

struct Profiler
{
  bool profiling;

  std::ofstream file;
  bool empty;

  void begin(const char *fname);
  void end();

  void write(const ProfileResult &result);

  static Profiler &get();
};

struct TimeScope
{
  const char *name;
  bool stopped;

  std::chrono::time_point<std::chrono::high_resolution_clock> start;

  TimeScope(const char *name_);
  ~TimeScope();

  void stop();
};