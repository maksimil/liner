#pragma once

#include "base.h"

template <> struct Load<double>
{
    static double structload(lstate L)
    {
        return lua_tonumber(L, -1);
    }
};

template <> struct Load<float>
{
    static float structload(lstate L)
    {
        return (float) load<double>(L);
    }
};

template <> struct Load<uint8_t>
{
    static uint8_t structload(lstate L)
    {
        return (uint8_t) lua_tointeger(L, -1);
    }
};

template <> struct Load<const char *>
{
    static const char *structload(lstate L)
    {
        return lua_tostring(L, -1);
    }
};

template <> struct Load<std::string>
{
    static std::string structload(lstate L)
    {
        return load<const char *>(L);
    }
};