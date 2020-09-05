#include "glua.h"
#include "../utils/profiler.h"
#include <iostream>

lstate newstate()
{
  lstate L = luaL_newstate();
  luaL_openlibs(L);

  lua_register(L, "log_event", [](lstate Lf) -> int {
    const char *message = lua_tostring(Lf, 1);
    Profiler::get().log(message);
    return 0;
  });

  lua_register(L, "log_value", [](lstate Lf) -> int {
    const char *name = lua_tostring(Lf, 1);
    const Value &args = load<Value>(Lf);
    Profiler::get().log(name, args);
    return 0;
  });

  return L;
}

bool runscript(lstate L, const std::string &fname)
{
  bool ok = luaL_dofile(L, fname.c_str()) == LUA_OK;
  if (!ok)
  {
    std::cout << lua_tostring(L, -1) << "\n";
  }
  return ok;
}

bool pcall(lstate L, const int &inargs, const int &outargs)
{
  bool ok = lua_pcall(L, inargs, outargs, 0) == LUA_OK;
  if (!ok)
  {
    std::cout << lua_tostring(L, -1) << "\n";
  }
  return ok;
}