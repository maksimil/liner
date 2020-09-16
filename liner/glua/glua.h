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

struct ValueRef
{
  std::string path;
  std::string name;
};

#define VALUE_VARIANT std::variant<double, std::string, Component>
// variant index
#define NUMBER_INDEX    0
#define STRING_INDEX    1
#define COMPONENT_INDEX 2

enum class ValueIndex
{
  number,
  string,
  component
};

struct Value
{
  Value();
  ~Value();

  Value(const ValueIndex &_type);
  Value(const Value &value);

  Value(const double &num);
  Value(const std::string &str);

  void operator=(const Value &other);

  VALUE_VARIANT *vl;

  Value &operator[](const std::string &key);
  const Value &operator[](const std::string &key) const;

  double &number();
  const double &number() const;

  std::string &string();
  const std::string &string() const;
  std::string tostring() const;

  Component &component();
  const Component &component() const;
};

lstate newstate();

// run lua script
bool runscript(lstate L, const std::string &fname);
bool runscript(lstate L, const char *fname);

void pushvalue(lstate L, const Value &value);

#define LOADDEF(Tn)                                                            \
  template <> Tn load<Tn>(const ValueRef &ref);                                \
  template <> Tn load<Tn>(lstate L, const ValueRef &ref);                      \
  template <> Tn load<Tn>(lstate L, const char *gname);                        \
  template <> Tn load<Tn>(lstate L);

template <typename T> T load(const ValueRef &ref);
template <typename T> T load(lstate L, const ValueRef &ref);
template <typename T> T load(lstate L, const char *gname);
template <typename T> T load(lstate L);

LOADDEF(Value)
LOADDEF(ValueRef)

Value instantiate(lstate L, const char *gname);
Value instantiate(lstate L);
Value instantiate(const Value &shape);

std::ostream &operator<<(std::ostream &cout, const Value &value);

std::vector<Value> runscript(const std::vector<Value> &values,
                             const char *fname, const char *function);

bool pcall(lstate L, const int &inargs, const int &outargs);

bool hasoption(const Value &value, const std::string &option);