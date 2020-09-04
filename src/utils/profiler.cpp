#include "profiler.h"
#include "time.h"
#include <iostream>

Profiler &Profiler::get()
{
  static Profiler instance;
  return instance;
}

void Profiler::begin(const char *fname)
{
  begin(fname, json);
}

void Profiler::begin(const char *fname, const writemode &wmode)
{
  if (profiling)
  {
    mode = wmode;
    empty = true;

    if (mode == json)
    {
      file.open(fname);
      file << "{\"otherData\": {},\"traceEvents\":[";
      file.flush();
    }
    else if (mode == bin)
    {
      file.open(fname, std::ios::binary | std::ios::out);
    }
  }
}

void Profiler::end()
{
  if (profiling && mode == json)
  {
    file << "]}";
  }
  file.close();
}

void Profiler::write(const ProfileResult &res)
{
  switch (mode)
  {
  case json:
  {
    if (empty)
    {
      empty = false;
    }
    else
    {
      file << ",";
    }

    file << "{";
    file << "\"cat\":\"function\",";
    file << "\"dur\":" << (res.end - res.start) << ',';
    file << "\"name\":\"" << res.name << "\",";
    file << "\"ph\":\"X\",";
    file << "\"pid\":0,";
    file << "\"tid\":" << res.tid << ",";
    file << "\"ts\":" << res.start;
    file << "}";

    break;
  }
  case bin:
  {
    size_t size = strlen(res.name);
    int64_t duration = res.end - res.start;

    file.write((char *) &size, sizeof(size_t));
    file.write(res.name, size);
    file.write((char *) &res.start, sizeof(int64_t));
    file.write((char *) &duration, sizeof(int64_t));
    file.write((char *) &res.tid, sizeof(uint32_t));

    break;
  }
  }
  file.flush();
}

TimeScope::TimeScope(const char *name_)
{
  name = name_;
  start = NOW;
  stopped = false;
}

void TimeScope::stop()
{
  Profiler &profiler = Profiler::get();
  if (profiler.profiling && !stopped)
  {
    ProfileResult res;
    res.tid = std::hash<std::thread::id>{}(std::this_thread::get_id());

    auto endtp = NOW;

    res.start = std::chrono::time_point_cast<std::chrono::microseconds>(start)
                    .time_since_epoch()
                    .count();
    res.end = std::chrono::time_point_cast<std::chrono::microseconds>(endtp)
                  .time_since_epoch()
                  .count();
    res.name = name;
    profiler.write(res);
    stopped = true;
  }
}

TimeScope::~TimeScope()
{
  stop();
}
