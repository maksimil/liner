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

#define COMMATHINGEY(vname)                                                    \
  if (vname)                                                                   \
  {                                                                            \
    vname = false;                                                             \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    file << ",";                                                               \
  }

void Profiler::log(const char *log)
{
  COMMATHINGEY(empty)

  file << "{";
  file << "\"cat\":\"log\",";
  file << "\"name\":\"" << log << "\",";
  file << "\"ph\":\"I\",";
  file << "\"pid\":0,";
  file << "\"tid\":" << std::hash<std::thread::id>{}(std::this_thread::get_id())
       << ",";
  file << "\"ts\":"
       << std::chrono::time_point_cast<std::chrono::microseconds>(NOW)
              .time_since_epoch()
              .count();
  file << "}";

  file.flush();
}

void Profiler::log(const char *name, const Value &args)
{
  COMMATHINGEY(empty)

  file << "{";
  file << "\"cat\":\"log\",";
  file << "\"name\":\"" << name << "\",";
  file << "\"ph\":\"C\",";
  file << "\"pid\":0,";
  file << "\"tid\":" << std::hash<std::thread::id>{}(std::this_thread::get_id())
       << ",";
  file << "\"ts\":"
       << std::chrono::time_point_cast<std::chrono::microseconds>(NOW)
              .time_since_epoch()
              .count()
       << ",";
  file << "\"args\":{";

  const Component &argsmap = *args.component();
  bool nfirst = true;
  for (auto &pair : argsmap)
  {
    COMMATHINGEY(nfirst)
    file << "\"" << pair.first << "\":\"" << pair.second.tostring() << "\"";
  }

  file << "}}";
}

void Profiler::write(const ProfileResult &res)
{
  COMMATHINGEY(empty)

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
