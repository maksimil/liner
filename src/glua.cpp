#include "glua.h"
#include <iostream>

bool runscript(lstate L, const str &fname)
{
  int r = luaL_dofile(L, fname.c_str());
  bool ok = r == LUA_OK;
  if (!ok)
  {
    std::cout << lua_tostring(L, -1) << "\n";
  }
  return ok;
}