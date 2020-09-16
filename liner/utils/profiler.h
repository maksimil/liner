#pragma once

#include "../glua/glua.h"
#include <fstream>
#include <future>
#include <string>
#include <vector>

#define CONCAT(a, b) a##b

#define TSCOPEID(name, id) TimeScope CONCAT(tsc, id)(name)
#define TSCOPE(name)       TSCOPEID(name, __LINE__)

struct ProfileResult
{
  int64_t start, end;
  const char *name;
  uint32_t tid;
};

struct Profiler
{
  bool profiling = false;

  std::ofstream file;
  std::mutex filemutex;
  bool empty = true;

  void begin(const char *fname);
  void end();

  void log(const char *message);
  void log(const char *name, const Value &args);
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