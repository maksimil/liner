#include "glua/glua.h"
#include "utils/profiler.h"
#include "utils/time.h"
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
  {
    writemode mode = json;
    if (hasoption(settings, "pformat"))
    {
      if (settings["pformat"].string() == "bin")
        mode = bin;
    }
    profiler.begin(settings["profiling"].string().c_str(), mode);
  }

  const Value shape = loadvalue(L, "shape");
  const Value update = loadvalue(L, "update");
  const Value init = loadvalue(L, "init");

  TSCOPEID("initialize", 27);
  CHECKRUN(L, shape["path"].string());
  Value state = instantiate(L, shape["vname"].string());
  tsc27.stop();

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
      TSCOPE("update");

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
      config << "    delta = 100,\n";
      config << "    profiling = \"profiler.json\",\n";
      config << "    pformat = \"json\"\n";
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

    if (strcmp(argv[1], "epr") == 0)
    {
      if (argc != 3)
      {
        std::cout << "Wrong number of arguments for command liner epr <file>";
        return 1;
      }
      std::string fname = argv[2];
      std::ifstream binfile(fname, std::ios::binary | std::ios::in);
      std::ofstream jsonfile(fname + ".json", std::ios::binary | std::ios::out);

      jsonfile << "{\"otherData\": {},\"traceEvents\":[";

      binfile.seekg(0, std::ios::end);
      int size = binfile.tellg();
      binfile.seekg(0, std::ios::beg);

      bool first = true;

      while (size != (int) binfile.tellg())
      {
        if (first)
        {
          first = false;
        }
        else
        {
          jsonfile << ",";
        }

        int64_t duration, start;
        uint32_t tid;
        size_t size;

        binfile.read((char *) &size, sizeof(size_t));
        std::string name = "";
        name.resize(size);
        binfile.read(&name[0], size);
        binfile.read((char *) &start, sizeof(int64_t));
        binfile.read((char *) &duration, sizeof(int64_t));
        binfile.read((char *) &tid, sizeof(uint32_t));

        jsonfile << "{";
        jsonfile << "\"cat\":\"function\",";
        jsonfile << "\"dur\":" << duration << ',';
        jsonfile << "\"name\":\"" << name << "\",";
        jsonfile << "\"ph\":\"X\",";
        jsonfile << "\"pid\":0,";
        jsonfile << "\"tid\":" << tid << ",";
        jsonfile << "\"ts\":" << start;
        jsonfile << "}";

        jsonfile.flush();
      }

      jsonfile << "]}";

      jsonfile.close();
      binfile.close();
    }
  }
  return 0;
}
