#pragma once

#include "../glua.h"

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