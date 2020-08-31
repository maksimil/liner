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
  st,
  num,
  comp,
};

struct Value
{
  Value();
  Value(const valuetypes &t);

  Value(const str &v);
  Value(const double &v);

  Value(const Value &value);

  Value operator=(const Value &v);

  valuetypes vtype;
  union {
    str string;
    double number;
    Component *component;
  };

  ~Value();
};

Value loadvalue(lstate L, const str &gname);
Value loadvalue(lstate L);