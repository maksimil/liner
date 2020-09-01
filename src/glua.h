#include <lua.hpp>
#include <map>
#include <string>
#include <variant>

#define lstate    lua_State *
#define Component std::map<std::string, Value>

enum valuetype
{
  str,
  num,
  comp
};

struct Value
{
  Value(const valuetype &_type);
  Value(const Value &value);

  Value(const double &num);
  Value(const std::string &str);

  valuetype type;
  std::variant<double, std::string, Component *> vl;
};

// run lua script
bool runscript(lstate L, const std::string &fname);

Value loadvalue(lstate L, const std::string &gname);
Value loadvalue(lstate L);