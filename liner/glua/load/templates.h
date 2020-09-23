#pragma once

#include "base.h"
#include <array>

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

template <size_t I, class... T>
void loadtuple(lstate L, std::tuple<T...> &tuple)
{
    if constexpr (I < sizeof...(T))
    {
        if (lua_next(L, -2))
        {
            std::get<I>(tuple) =
                load<std::tuple_element_t<I, std::tuple<T...>>>(L);
            lua_pop(L, 1);

            loadtuple<I + 1, T...>(L, tuple);
        }
    }
    else if (lua_next(L, -2) != 0)
    {
        lua_pop(L, 2);
    }
}

template <class... T> struct Load<std::tuple<T...>>
{
    static std::tuple<T...> structload(lstate L)
    {
        std::tuple<T...> res;
        lua_pushnil(L);
        loadtuple<0, T...>(L, res);
        return res;
    }
};