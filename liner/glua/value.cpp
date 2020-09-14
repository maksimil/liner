#include "glua.h"

#define NUMBER "number"
#define STRING "string"

// Value struct functions

Value::Value(const valuetype &_type) : type(_type)
{
  switch (type)
  {
  case num:
    vl = 0;
    break;
  case str:
    vl = "";
    break;
  case comp:
    vl = new Component;
    break;
  }
}

Value::Value(const Value &value) : type(value.type)
{
  switch (type)
  {
  case num:
  case str:
    vl = value.vl;
    break;
  case comp:
    vl = new Component;
    *component() = *value.component();
    break;
  }
}

Value::Value(const double &number) : type(num)
{
  vl = number;
}

Value::Value(const std::string &string) : type(str)
{
  vl = string;
}

Value &Value::operator[](const std::string &key)
{
  return component()->at(key);
}

const Value &Value::operator[](const std::string &key) const
{
  return component()->at(key);
}

double &Value::number()
{
  return std::get<double>(vl);
}

const double &Value::number() const
{
  return std::get<double>(vl);
}

std::string &Value::string()
{
  return std::get<std::string>(vl);
}

const std::string &Value::string() const
{
  return std::get<std::string>(vl);
}

Component *&Value::component()
{
  return std::get<Component *>(vl);
}

const Component *Value::component() const
{
  return std::get<Component *>(vl);
}

std::string Value::tostring() const
{
  switch (type)
  {
  case num:
    return std::to_string(number());
    break;
  case str:
    return string();
    break;
  case comp:
    return "object";
    break;
  }
  return "";
}

// Value functions

void pushvalue(lstate L, const Value &value)
{
  switch (value.type)
  {
  case num:
    lua_pushnumber(L, value.number());
    break;
  case str:
    lua_pushstring(L, value.string().c_str());
    break;
  case comp:
    lua_newtable(L);
    for (auto &pair : *value.component())
    {
      lua_pushstring(L, pair.first.c_str());
      pushvalue(L, pair.second);
      lua_settable(L, -3);
    }
    break;
  }
}

void printvalue(std::ostream &cout, const Value &value,
                const std::string &indent = "")
{
  switch (value.type)
  {
  case str:
    cout << "\"" << value.string() << "\"";
    break;
  case num:
    cout << value.number();
    break;
  case comp:
    const Component &component = *value.component();
    const std::string newindent = indent + "  ";
    cout << "{\n";
    for (auto &pair : component)
    {
      cout << newindent << pair.first << ": ";
      printvalue(cout, pair.second, newindent);
      cout << ",\n";
    }
    cout << indent << "}";
  }
};

std::ostream &operator<<(std::ostream &cout, const Value &value)
{
  printvalue(cout, value);
  return cout;
}

bool hasoption(const Value &value, const std::string &option)
{
  return value.component()->count(option);
}

Value instantiate(lstate L, const char *gname)
{
  lua_getglobal(L, gname);
  const Value res = instantiate(L);
  lua_pop(L, 1);
  return res;
}

Value instantiate(lstate L)
{
  return instantiate(load<Value>(L));
}

Value instantiate(const Value &shape)
{
  switch (shape.type)
  {
  case str:
  {
    const std::string key = shape.string();
    if (key == NUMBER)
      return 0;
    if (key == STRING)
      return Value("");
    break;
  }

  case comp:
  {
    Value res(comp);
    Component *component = res.component();
    for (auto &pair : *shape.component())
    {
      component->insert({pair.first, instantiate(pair.second)});
    }
    return res;
    break;
  }
  }
  return 0;
}