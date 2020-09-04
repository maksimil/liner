#include "glua.h"
#include <iostream>

lstate newstate()
{
  lstate L = luaL_newstate();
  luaL_openlibs(L);
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