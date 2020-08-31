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

Value::Value()
{
  vtype = num;
  number = 0;
}

Value::Value(const valuetypes &t)
{
  vtype = t;
  number = 0;
  if (vtype == comp)
  {
    component = new Component;
  }
}

Value::Value(const str &v)
{
  vtype = st;
  string = v;
}

Value::Value(const double &v)
{
  vtype = num;
  number = v;
}

Value::Value(const Value &v)
{
  vtype = v.vtype;
  switch (vtype)
  {
  case st:
    string = v.string;
    break;
  case num:
    number = v.number;
    break;
  case comp:
    component = new Component;
    for (const auto pair : *v.component)
    {
      component->insert(pair);
    }
    break;

  default:
    break;
  }
}

Value Value::operator=(const Value &v)
{
  return Value(v);
}

Value::~Value()
{
  if (vtype == comp)
  {
    delete component;
  }
}