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
  if (profiling)
  {
    empty = true;

    file.open(fname);
    file << "{\"otherData\":{},\"traceEvents\":[";
    file.flush();
  }
}

void Profiler::end()
{
  file << "]}";
  file.close();
}

void Profiler::write(const ProfileResult &res)
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
