#include "glua.h"

void runtask(const Value *value, const std::string fname,
             const std::string function, std::mutex *mutex,
             std::vector<Value> *vs)
{
  lstate L = luaL_newstate();
  if (runscript(L, fname))
  {
    lua_getglobal(L, function.c_str());
    // lua_pushinteger(L, (int) value);
    if (!pcall(L, 0, 1))
    {
      std::lock_guard<std::mutex> lk(*mutex);
      vs->emplace_back(0);
      return;
    }
    Value v = loadvalue(L);
    std::lock_guard<std::mutex> lk(*mutex);
    vs->emplace_back(v);
  }
  lua_close(L);
}

std::vector<Value> runscript(const std::vector<Value> &values,
                             const std::string &fname,
                             const std::string &function)
{
  std::vector<Value> vs = {};
  {
    std::vector<std::future<void>> futs = {};
    futs.reserve(values.size());
    vs.reserve(values.size());

    std::mutex mutex;

    for (size_t i = 0; i < values.size(); i++)
    {
      futs.push_back(std::async(std::launch::async, runtask, &values[i], fname,
                                function, &mutex, &vs));
    }
  }
  return vs;
}