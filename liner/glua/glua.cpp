#include "glua.h"
#include "../renderer/renderer.h"
#include "../utils/console.h"
#include "../utils/profiler.h"
#include "load.h"
#include <iostream>

lstate newstate()
{
    lstate L = luaL_newstate();
    luaL_openlibs(L);

    // log event in profiler
    lua_register(L, "log_event", [](lstate Lf) -> int {
        const char *message = lua_tostring(Lf, 1);
        Profiler::get().log(message);
        return 0;
    });

    // log value in profiler
    lua_register(L, "log_value", [](lstate Lf) -> int {
        const char *name = lua_tostring(Lf, 1);
        const Value &args = load<Value>(Lf);
        Profiler::get().log(name, args);
        return 0;
    });

    // log in console
    lua_register(L, "log_console", [](lstate Lf) -> int {
        LOG(lua_tostring(Lf, 1));
        return 0;
    });

    // draw line
    lua_register(L, "draw_line", [](lstate Lf) -> int {
        Line line;

        lua_pushnil(Lf);
        while (lua_next(Lf, -2) != 0)
        {
            lua_pushnil(Lf);

            // x
            lua_next(Lf, -2);
            const float x = (float) lua_tonumber(Lf, -1);
            lua_pop(Lf, 1);

            // y
            lua_next(Lf, -2);
            const float y = (float) lua_tonumber(Lf, -1);
            lua_pop(Lf, 2);

            line.push_back({{x, y}, sf::Color::White});

            lua_pop(Lf, 1);
        }

        Renderer::get().draw(line);

        return 0;
    });

    // close window
    lua_register(L, "window_close", [](lstate Lf) -> int {
        Renderer::get().window->close();
        return 0;
    });

    return L;
}

bool runscript(lstate L, const std::string &fname)
{
    return runscript(L, fname.c_str());
}

bool runscript(lstate L, const char *fname)
{
    bool ok = luaL_dofile(L, fname) == LUA_OK;
    if (!ok)
    {
        std::cout << lua_tostring(L, -1) << "\n";
    }
    return ok;
}

bool pcall(lstate L, const int &inargs, const int &outargs)
{
    bool ok = lua_pcall(L, inargs, outargs, 0) == LUA_OK;
    if (!ok)
    {
        std::cout << lua_tostring(L, -1) << "\n";
    }
    return ok;
}