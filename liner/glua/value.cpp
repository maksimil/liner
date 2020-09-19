#include "glua.h"
#include "load.h"

#define NUMBER "number"
#define STRING "string"

// Value struct functions

Value::Value()
{
    vl = nullptr;
}

Value::~Value()
{
    delete vl;
}

Value::Value(const ValueIndex &index)
{
    vl = nullptr;
    switch (index)
    {
    case ValueIndex::number:
        vl = new VALUE_VARIANT(0);
        break;
    case ValueIndex::string:
        vl = new VALUE_VARIANT("");
        break;
    case ValueIndex::component:
        vl = new VALUE_VARIANT(std::map<std::string, Value>{});
        break;
    }
}

Value::Value(const Value &value)
{
    vl = new VALUE_VARIANT(*value.vl);
}

Value::Value(const double &number)
{
    vl = new VALUE_VARIANT(number);
}

Value::Value(const std::string &string)
{
    vl = new VALUE_VARIANT(string);
}

Value &Value::operator[](const std::string &key)
{
    return component().at(key);
}

const Value &Value::operator[](const std::string &key) const
{
    return component().at(key);
}

double &Value::number()
{
    return std::get<double>(*vl);
}

const double &Value::number() const
{
    return std::get<double>(*vl);
}

std::string &Value::string()
{
    return std::get<std::string>(*vl);
}

const std::string &Value::string() const
{
    return std::get<std::string>(*vl);
}

Component &Value::component()
{
    return std::get<Component>(*vl);
}

const Component &Value::component() const
{
    return std::get<Component>(*vl);
}

std::string Value::tostring() const
{
    switch (vl->index())
    {
    case NUMBER_INDEX:
        return std::to_string(number());
        break;
    case STRING_INDEX:
        return string();
        break;
    case COMPONENT_INDEX:
        return "object";
        break;
    }
    return "";
}

// Value functions
void Value::operator=(const Value &other)
{
    *vl = *other.vl;
}

void pushvalue(lstate L, const Value &value)
{
    switch (value.vl->index())
    {
    case NUMBER_INDEX:
        lua_pushnumber(L, value.number());
        break;
    case STRING_INDEX:
        lua_pushstring(L, value.string().c_str());
        break;
    case COMPONENT_INDEX:
        lua_newtable(L);
        for (auto &pair : value.component())
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
    switch (value.vl->index())
    {
    case STRING_INDEX:
        cout << "\"" << value.string() << "\"";
        break;
    case NUMBER_INDEX:
        cout << value.number();
        break;
    case COMPONENT_INDEX:
        const std::string newindent = indent + "  ";
        cout << "{\n";
        for (auto &pair : value.component())
        {
            cout << newindent << pair.first << ": ";
            printvalue(cout, pair.second, newindent);
            cout << ",\n";
        }
        cout << indent << "}";
        break;
    }
};

std::ostream &operator<<(std::ostream &cout, const Value &value)
{
    printvalue(cout, value);
    return cout;
}

bool hasoption(const Value &value, const std::string &option)
{
    return value.component().count(option);
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
    switch (shape.vl->index())
    {
    case STRING_INDEX:
    {
        const std::string key = shape.string();
        if (key == NUMBER)
            return Value(0);
        if (key == STRING)
            return Value("");
        break;
    }

    case COMPONENT_INDEX:
    {
        Value res(ValueIndex::component);
        Component &component = res.component();
        for (auto &pair : shape.component())
        {
            component.insert({pair.first, instantiate(pair.second)});
        }
        return res;
        break;
    }
    }
    return Value(0);
}