#include <lua.hpp>

using str = std::string;
using lstate = lua_State *;

// run lua script
bool runscript(lstate L, const str &fname);