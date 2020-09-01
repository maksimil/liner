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
    *getcomp(vl) = *getcomp(value.vl);
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
  switch (lua_type(L, -1))
  {
  case LUA_TSTRING:
    return str;
    break;
  case LUA_TNUMBER:
    return num;
    break;
  case LUA_TTABLE:
    return comp;
    break;
  }
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
      getcomp(res.vl)->insert(std::pair{key, value});
      lua_pop(L, 1);
    }
    break;
  }
  return res;
}

void printvalue(std::ostream &cout, const Value &value,
                const std::string &indent = "")
{
  switch (value.type)
  {
  case str:
    cout << "\"" << getstr(value.vl) << "\"";
    break;
  case num:
    cout << getnum(value.vl);
    break;
  case comp:
    const Component &component = *getcomp(value.vl);
    const std::string newindent = indent + "  ";
    cout << "{\n";
    for (auto &&pair : component)
    {
      cout << newindent << pair.first << ": ";
      printvalue(cout, pair.second, newindent);
      cout << ",\n";
    }
    cout << indent << "}";
  }
};

std::ostream &operator<<(std::ostream &cout, const Value &value)
{
  printvalue(cout, value);
  return cout;
}