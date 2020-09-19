#pragma once

#include "glua.h"

template <typename T> inline T load(const ValueRef &ref)
{
    lstate L = newstate();
    const T res = load<T>(L, ref);
    lua_close(L);
    return res;
}

template <typename T> inline T load(lstate L, const ValueRef &ref)
{
    runscript(L, ref.path);
    return load<T>(L, ref.name.c_str());
}

template <typename T> inline T load(lstate L, const char *gname)
{
    lua_getglobal(L, gname);
    return load<T>(L);
}
template <typename T> inline T load(lstate L)
{
    return T();
}

inline uint8_t gettype(lstate L)
{
    switch (lua_type(L, -1))
    {
    case LUA_TSTRING:
        return STRING_INDEX;
        break;
    case LUA_TNUMBER:
        return NUMBER_INDEX;
        break;
    case LUA_TTABLE:
        return COMPONENT_INDEX;
        break;
    }
    return NUMBER_INDEX;
}

template <> inline Value load<Value>(lstate L)
{
    switch (gettype(L))
    {
    case NUMBER_INDEX:
        return lua_tonumber(L, -1);
        break;
    case STRING_INDEX:
        return Value(lua_tostring(L, -1));
        break;
    case COMPONENT_INDEX:
        Value res(ValueIndex::component);
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            const std::string key = lua_tostring(L, -2);
            const Value value = load<Value>(L);
            res.component().insert(std::pair{key, value});
            lua_pop(L, 1);
        }
        return res;
        break;
    }
    return Value(0);
}

template <> inline ValueRef load<ValueRef>(lstate L)
{
    ValueRef ref;
    lua_pushstring(L, "path");
    lua_gettable(L, -2);
    ref.path = lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "name");
    lua_gettable(L, -2);
    ref.name = lua_tostring(L, -1);
    lua_pop(L, 1);

    return ref;
}