#pragma once

#include "SFML/Graphics/Vertex.hpp"
#include "base.h"
#include <array>

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

template <> struct Load<sf::Color>
{
    static sf::Color structload(lstate L)
    {
        sf::Color res;

        auto rgb = load<std::array<uint8_t, 3>>(L);

        res.r = rgb[0];
        res.g = rgb[1];
        res.b = rgb[2];

        return res;
    };
};

template <> struct Load<sf::Vector2f>
{
    static sf::Vector2f structload(lstate L)
    {
        auto vs = load<std::tuple<float, float>>(L);
        return {std::get<0>(vs), std::get<1>(vs)};
    }
};

template <> struct Load<sf::Vertex>
{
    static sf::Vertex structload(lstate L)
    {
        sf::Vertex res;

        auto vs = load<std::tuple<sf::Vector2f, sf::Color>>(L);

        res.position = std::get<0>(vs);
        res.color = std::get<1>(vs);

        return res;
    }
};