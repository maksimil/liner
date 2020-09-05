#pragma once

#include <future>
#include <iostream>
#include <lua.hpp>
#include <map>
#include <string>
#include <variant>
#include <vector>

#define lstate    lua_State *
#define Component std::map<std::string, Value>

#define CHECKRUN(L, name)                                                      \
  if (!runscript(L, name))                                                     \
  {                                                                            \
    lua_close(L);                                                              \
    return;                                                                    \
  }

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

  Value &operator[](const std::string &key);
  const Value &operator[](const std::string &key) const;

  double &number();
  const double &number() const;

  std::string &string();
  const std::string &string() const;
  std::string tostring() const;

  Component *&component();
  const Component *component() const;
};

lstate newstate();

// run lua script
bool runscript(lstate L, const std::string &fname);

void pushvalue(lstate L, const Value &value);

Value loadvalue(lstate L, const std::string &gname);
Value loadvalue(lstate L);

Value instantiate(lstate L, const std::string &gname);
Value instantiate(lstate L);
Value instantiate(const Value &shape);

std::ostream &operator<<(std::ostream &cout, const Value &value);

std::vector<Value> runscript(const std::vector<Value> &values,
                             const std::string &fname,
                             const std::string &function);

bool pcall(lstate L, const int &inargs, const int &outargs);

bool hasoption(const Value &value, const std::string &option);