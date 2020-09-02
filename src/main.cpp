#include "glua.h"
#include "time.h"
#include <fstream>
#include <iostream>
#include <thread>

void runmain()
{
  // read
  lstate L = newstate();

  CHECKRUN(L, "config.lua");

  const Value settings = loadvalue(L, "settings");
  const Value shape = loadvalue(L, "shape");
  const Value update = loadvalue(L, "update");

  CHECKRUN(L, shape["path"].string());
  Value state = instantiate(L, shape["vname"].string());

  CHECKRUN(L, update["path"].string());
  const std::string updatename = update["vname"].string();

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
  lua_close(L);
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
      config << "}\n";
      config << "\n";
      config << "shape = {\n";
      config << "    path = \"f.lua\",\n";
      config << "    vname = \"shape\"\n";
      config << "}\n";
      config << "\n";
      config << "update = {\n";
      config << "    path = \"f.lua\",\n";
      config << "    vname = \"upd\"\n";
      config << "}\n";
      config.close();

      std::ofstream f("f.lua");
      f << "shape = {}\n";
      f << "\n";
      f << "upd = function(obj)\n";
      f << "    print(\"HW\")\n";
      f << "end\n";
      f.close();
    }
  }
  return 0;
}
