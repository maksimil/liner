#include "glua.h"
#include "profiler.h"
#include "time.h"
#include <fstream>
#include <iostream>
#include <thread>

void runmain()
{
  // read
  lstate L = newstate();

  CHECKRUN(L, "config.lua");

  // settings
  const Value settings = loadvalue(L, "settings");
  // profiler
  Profiler &profiler = Profiler::get();
  profiler.profiling = hasoption(settings, "profiling");
  if (profiler.profiling)
    profiler.begin(settings["profiling"].string().c_str());

  const Value shape = loadvalue(L, "shape");
  const Value update = loadvalue(L, "update");
  const Value init = loadvalue(L, "init");

  TSCOPE(initialize);
  CHECKRUN(L, shape["path"].string());
  Value state = instantiate(L, shape["vname"].string());
  initialize.stop();

  CHECKRUN(L, update["path"].string());
  const std::string updatename = update["vname"].string();

  CHECKRUN(L, init["path"].string());
  lua_getglobal(L, init["vname"].string().c_str());
  pushvalue(L, state);
  if (!pcall(L, 1, 1))
  {
    lua_close(L);
    return;
  }
  state = loadvalue(L);
  lua_pop(L, 1);

  // run
  bool running = true;
  const MSTYPE period = MSTYPE((int) settings["delta"].number());
  auto lastupdate = NOW;

  while (running)
  {
    auto now = NOW;
    auto duration = DURMS(now - lastupdate);
    if (duration < period)
    {
      std::this_thread::sleep_for(period - duration);
    }
    lastupdate = NOW;

    {
      TSCOPE(update);

      lua_getglobal(L, updatename.c_str());
      pushvalue(L, state);
      if (!pcall(L, 1, 1))
      {
        lua_close(L);
        return;
      }
      state = loadvalue(L);
      lua_pop(L, 1);
    }
  }
  lua_close(L);

  profiler.end();
}

int main(int argc, char const *argv[])
{
  if (argc == 1)
  {
    runmain();
  }
  else
  {
    if (strcmp(argv[1], "init") == 0)
    {
      std::ofstream config("config.lua");
      config << "settings = {\n";
      config << "    delta = 100\n";
      config << "    profiling = \"profiler.json\"\n";
      config << "}\n\n";
      config << "shape = {\n";
      config << "    path = \"f.lua\",\n";
      config << "    vname = \"shape\"\n";
      config << "}\n\n";
      config << "update = {\n";
      config << "    path = \"f.lua\",\n";
      config << "    vname = \"update\"\n";
      config << "}\n\n";
      config << "init = {\n";
      config << "    path = \"f.lua\",\n";
      config << "    vname = \"init\"\n";
      config << "}\n";
      config.close();

      std::ofstream f("f.lua");
      f << "shape = {}\n\n";
      f << "init = function(obj)\n";
      f << "    print(\"init\")\n";
      f << "    return obj\n";
      f << "end\n\n";
      f << "update = function(obj)\n";
      f << "    print(\"HW\")\n";
      f << "    return obj\n";
      f << "end\n";
      f.close();
    }
  }
  return 0;
}
