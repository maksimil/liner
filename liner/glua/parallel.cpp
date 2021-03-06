#include "glua.h"
#include "load.h"

void runtask(const Value *value, const char *fname, const char *function,
             std::mutex *mutex, std::vector<Value> *vs)
{
    lstate L = newstate();
    if (runscript(L, fname))
    {
        lua_getglobal(L, function);
        pushvalue(L, *value);
        if (!pcall(L, 1, 1))
        {
            std::lock_guard<std::mutex> lk(*mutex);
            vs->emplace_back(0);
            return;
        }
        Value v = load<Value>(L);
        std::lock_guard<std::mutex> lk(*mutex);
        vs->emplace_back(v);
    }
    lua_close(L);
}

std::vector<Value> runscript(const std::vector<Value> &values,
                             const char *fname, const char *function)
{
    std::vector<Value> vs = {};
    {
        std::vector<std::future<void>> futs = {};
        futs.reserve(values.size());
        vs.reserve(values.size());

        std::mutex mutex;

        for (size_t i = 0; i < values.size(); i++)
        {
            futs.push_back(std::async(std::launch::async, runtask, &values[i],
                                      fname, function, &mutex, &vs));
        }
    }
    return vs;
}