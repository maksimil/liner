#include <lua.hpp>
#include <map>
#include <string>

#define str       std::string
#define lstate    lua_State *
#define Component std::map<str, Value>

// run lua script
bool runscript(lstate L, const str &fname);

enum valuetypes
{
  string,
  number,
  component,
};

struct Value
{
  valuetypes vtype;
  union {
    str string;
    double number;
    Component *component;
  };

  ~Value();
};

struct Shape
{
  std::map<str, str> shape;
  Component value;
};