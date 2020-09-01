#include <future>
#include <iostream>
#include <lua.hpp>
#include <map>
#include <string>
#include <variant>
#include <vector>

#define lstate    lua_State *
#define Component std::map<std::string, Value>

#define getstr  std::get<std::string>
#define getnum  std::get<double>
#define getcomp std::get<Component *>

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

std::ostream &operator<<(std::ostream &cout, const Value &value);

std::vector<Value> runscript(const std::vector<Value> &values,
                             const std::string &fname,
                             const std::string &function);

bool pcall(lstate L, const int &inargs, const int &outargs);