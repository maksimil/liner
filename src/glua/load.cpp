#include "glua.h"

#define IMPLEMENTLOAD(Tn)                                                      \
  template <> Tn load<Tn>(const ValueRef &ref)                                 \
  {                                                                            \
    lstate L = newstate();                                                     \
    const Tn res = load<Tn>(L, ref);                                           \
    lua_close(L);                                                              \
    return res;                                                                \
  }                                                                            \
                                                                               \
  template <> Tn load<Tn>(lstate L, const ValueRef &ref)                       \
  {                                                                            \
    runscript(L, ref.file);                                                    \
    return load<Tn>(L, ref.name);                                              \
  }                                                                            \
                                                                               \
  template <> Tn load<Tn>(lstate L, const char *gname)                         \
  {                                                                            \
    lua_getglobal(L, gname);                                                   \
    return load<Tn>(L);                                                        \
  }

IMPLEMENTLOAD(Value)

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

template <> Value load<Value>(lstate L)
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
      const Value value = load<Value>(L);
      res.component()->insert(std::pair{key, value});
      lua_pop(L, 1);
    }
    break;
  }
  return res;
}