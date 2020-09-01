#include "glua.h"
#include <iostream>

bool runscript(lstate L, const std::string &fname)
{
  int r = luaL_dofile(L, fname.c_str());
  bool ok = r == LUA_OK;
  if (!ok)
  {
    std::cout << lua_tostring(L, -1) << "\n";
  }
  return ok;
}

Value::Value(const valuetype &_type) : type(_type)
{
  switch (type)
  {
  case num:
    vl = 0;
    break;
  case str:
    vl = "";
    break;
  case comp:
    vl = new Component;
    break;
  }
}

Value::Value(const Value &value) : type(value.type)
{
  switch (type)
  {
  case num:
  case str:
    vl = value.vl;
    break;
  case comp:
    vl = new Component;
    *std::get<Component *>(vl) = *std::get<Component *>(value.vl);
    break;
  }
}

Value::Value(const double &number) : type(num)
{
  vl = number;
}

Value::Value(const std::string &string) : type(str)
{
  vl = string;
}

Value loadvalue(lstate L, const std::string &str)
{
  lua_getglobal(L, str.c_str());
  const Value res = loadvalue(L);
  lua_pop(L, 1);
  return res;
}

valuetype gettype(lstate L)
{
  if (lua_istable(L, -1))
    return comp;
  if (lua_isstring(L, -1))
    return str;
  // if (lua_isnumber(L, -1))
  return num;
}

Value loadvalue(lstate L)
{
  Value res(gettype(L));
  switch (res.type)
  {
  case num:
    res.vl = lua_tonumber(L, -1);
    break;
  case str:
    res.vl = lua_tostring(L, -1);
    break;
  case comp:
    lua_pushnil(L);
    while (lua_next(L, -2) != 0)
    {
      const std::string key = lua_tostring(L, -2);
      const Value value = loadvalue(L);
      std::get<Component *>(res.vl)->insert(std::pair{key, value});
      lua_pop(L, 1);
    }
    break;
  }
  return res;
}