#include "glua.h"

#define NUMBER "number"
#define STRING "string"

Value instantiate(lstate L, const std::string &gname)
{
  lua_getglobal(L, gname.c_str());
  const Value res = instantiate(L);
  lua_pop(L, 1);
  return res;
}

Value instantiate(lstate L)
{
  return instantiate(loadvalue(L));
}

Value instantiate(const Value &shape)
{
  switch (shape.type)
  {
  case str:
  {
    const std::string key = shape.string();
    if (key == NUMBER)
      return 0;
    if (key == STRING)
      return "";
    break;
  }

  case comp:
  {
    Value res(comp);
    Component *component = res.component();
    for (auto &pair : *shape.component())
    {
      component->insert({pair.first, instantiate(pair.second)});
    }
    return res;
    break;
  }
  }
  return 0;
}