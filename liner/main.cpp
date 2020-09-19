#include "glua/glua.h"
#include "glua/load.h"
#include "renderer/renderer.h"
#include "utils/profiler.h"
#include "utils/time.h"
#include <fstream>
#include <iostream>
#include <thread>

void runmain()
{
    TSCOPEID("startup", 11);
    // read
    lstate L = newstate();

    CHECKRUN(L, "config.lua");

    // settings
    const Value settings = load<Value>(L, "settings");

    // profiler
    Profiler &profiler = Profiler::get();
    profiler.profiling = hasoption(settings, "profiling");
    if (profiler.profiling)
    {
        const std::string &fname = settings["profiling"].string();
        if (fname != "")
            profiler.begin(fname.c_str());
    }

    // renderer
    const Value &windowsettings = settings["window"];

    const char *windowtitle = windowsettings["title"].string().c_str();
    const uint32_t &windowwidth = (uint32_t) windowsettings["width"].number();
    const uint32_t &windowheight = (uint32_t) windowsettings["height"].number();

    std::future<void> renderinit =
        Renderer::get().begin(windowtitle, windowwidth, windowheight);

    TSCOPEID("initialize state", 27);
    const ValueRef shape = load<ValueRef>(L, "shape");
    const ValueRef update = load<ValueRef>(L, "update");
    const ValueRef init = load<ValueRef>(L, "init");

    CHECKRUN(L, update.path);
    const std::string &updatename = update.name;

    CHECKRUN(L, shape.path);
    Value state = instantiate(L, shape.name.c_str());

    CHECKRUN(L, init.path);
    lua_getglobal(L, init.name.c_str());
    pushvalue(L, state);
    if (!pcall(L, 1, 1))
    {
        lua_close(L);
        return;
    }
    state = load<Value>(L);
    lua_pop(L, 1);
    tsc27.stop();

    // run
    bool running = true;
    const MSTYPE period = MSTYPE((int) settings["delta"].number());
    auto lastupdate = NOW;

    renderinit.wait();

    tsc11.stop();

    // main loop
    Renderer &renderer = Renderer::get();
    while (running)
    {
        WAIT_UNTIL(lastupdate, period)

        {
            TSCOPE("update");

            lua_getglobal(L, updatename.c_str());
            pushvalue(L, state);
            if (!pcall(L, 1, 1))
            {
                lua_close(L);
                return;
            }
            state = load<Value>(L);
            lua_pop(L, 1);

            renderer.render();
        }
    }
    lua_close(L);

    profiler.end();
}

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        runmain();
    }
    else
    {
        if (strcmp(argv[1], "init") == 0)
        {
            std::ofstream config("config.lua");
            config << "settings = {\n";
            config << "    delta = 100,\n";
            config << "    profiling = \"profiler.json\",\n";
            config << "}\n\n";
            config << "shape = {\n";
            config << "    path = \"f.lua\",\n";
            config << "    name = \"shape\"\n";
            config << "}\n\n";
            config << "update = {\n";
            config << "    path = \"f.lua\",\n";
            config << "    name = \"update\"\n";
            config << "}\n\n";
            config << "init = {\n";
            config << "    path = \"f.lua\",\n";
            config << "    name = \"init\"\n";
            config << "}\n";
            config.close();

            std::ofstream f("f.lua");
            f << "shape = {}\n\n";
            f << "init = function(obj)\n";
            f << "    print(\"init\")\n";
            f << "    return obj\n";
            f << "end\n\n";
            f << "update = function(obj)\n";
            f << "    print(\"HW\")\n";
            f << "    return obj\n";
            f << "end\n";
            f.close();
        }
    }
    return 0;
}
