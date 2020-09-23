#pragma once

#include "base.h"

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
    {
        lua_pushnil(L);
        const uint8_t res = lua_next(L, -2);

        if (res == 0)
            return LIST_INDEX;

        const int type = lua_type(L, -2);

        lua_pop(L, 2);

        switch (type)
        {
        case LUA_TNUMBER:
            return LIST_INDEX;
            break;
        case LUA_TSTRING:
            return COMPONENT_INDEX;
            break;
        }
    }
    break;
    }
    return NUMBER_INDEX;
}

template <> struct Load<Value>
{
    static Value structload(lstate L)
    {
        switch (gettype(L))
        {
        case NUMBER_INDEX:
            return load<double>(L);
            break;
        case STRING_INDEX:
            return Value(load<const char *>(L));
            break;
        case COMPONENT_INDEX:
        {
            Value res(ValueIndex::component);
            res.component() = load<Component>(L);
            return res;
        }
        break;
        case LIST_INDEX:
        {
            Value res(ValueIndex::list);
            res.list() = load<List>(L);
            return res;
        }
        break;
        }
        return Value(0);
    }
};

template <> struct Load<ValueRef>
{
    static ValueRef structload(lstate L)
    {
        ValueRef ref;
        lua_pushstring(L, "path");
        lua_gettable(L, -2);
        ref.path = load<const char *>(L);
        lua_pop(L, 1);

        lua_pushstring(L, "name");
        lua_gettable(L, -2);
        ref.name = load<const char *>(L);
        lua_pop(L, 1);

        return ref;
    }
};