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
    *component() = *value.component();
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

double &Value::number()
{
  return std::get<double>(vl);
}

const double &Value::number() const
{
  return std::get<double>(vl);
}

std::string &Value::string()
{
  return std::get<std::string>(vl);
}

const std::string &Value::string() const
{
  return std::get<std::string>(vl);
}

Component *&Value::component()
{
  return std::get<Component *>(vl);
}

const Component *Value::component() const
{
  return std::get<Component *>(vl);
}

void pushvalue(lstate L, const Value &value)
{
  switch (value.type)
  {
  case num:
    lua_pushnumber(L, value.number());
    break;
  case str:
    lua_pushstring(L, value.string().c_str());
    break;
  case comp:
    lua_newtable(L);
    for (auto &pair : *value.component())
    {
      lua_pushstring(L, pair.first.c_str());
      pushvalue(L, pair.second);
      lua_settable(L, -3);
    }
    break;
  }
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
      res.component()->insert(std::pair{key, value});
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
    cout << "\"" << value.string() << "\"";
    break;
  case num:
    cout << value.number();
    break;
  case comp:
    const Component &component = *value.component();
    const std::string newindent = indent + "  ";
    cout << "{\n";
    for (auto &pair : component)
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

bool pcall(lstate L, const int &inargs, const int &outargs)
{
  bool ok = lua_pcall(L, inargs, outargs, 0) == LUA_OK;
  if (!ok)
  {
    std::cout << lua_tostring(L, -1) << "\n";
  }
  return ok;
}