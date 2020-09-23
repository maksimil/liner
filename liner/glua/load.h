#pragma once

#include "glua.h"
#include <array>

template <class T> T load(const ValueRef &);
template <class T> T load(lstate, const ValueRef &);
template <class T> T load(lstate, const char *);
template <class T> T load(lstate, const int &);
template <class T> T load(lstate);

// have to use a class for partial specilization
template <class T> struct Load
{
    static T structload(lstate L)
    {
        return T();
    }
};

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

template <class K, class T> struct Load<std::map<K, T>>
{
    static std::map<K, T> structload(lstate L)
    {
        std::map<K, T> res = {};
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            const K key = load<K>(L, -2);
            const T value = load<T>(L);
            res.insert(std::pair<K, T>{key, value});
            lua_pop(L, 1);
        }
        return res;
    }
};

template <class T> struct Load<std::vector<T>>
{

    static std::vector<T> structload(lstate L)
    {
        std::vector<T> res = {};
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            const T value = load<T>(L);
            res.push_back(value);
            lua_pop(L, 1);
        }
        return res;
    }
};

template <class T, size_t S> struct Load<std::array<T, S>>
{
    static std::array<T, S> structload(lstate L)
    {
        std::array<T, S> res;

        lua_pushnil(L);
        size_t i = 0;

        while (lua_next(L, -2) != 0)
        {
            res[i] = load<T>(L);
            lua_pop(L, 1);
            i++;
        }

        return res;
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

template <class T> inline T load(const ValueRef &ref)
{
    lstate L = newstate();
    const T res = load<T>(L, ref);
    lua_close(L);
    return res;
}

template <class T> inline T load(lstate L, const ValueRef &ref)
{
    runscript(L, ref.path);
    return load<T>(L, ref.name.c_str());
}

template <class T> inline T load(lstate L, const char *gname)
{
    lua_getglobal(L, gname);
    const T res = load<T>(L);
    lua_pop(L, 1);
    return res;
}

template <class T> inline T load(lstate L, const int &index)
{
    lua_pushvalue(L, index);
    const T res = load<T>(L);
    lua_pop(L, 1);
    return res;
}

template <class T> inline T load(lstate L)
{
    return Load<T>::structload(L);
}
