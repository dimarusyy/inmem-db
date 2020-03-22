#pragma once
#include <boost/any.hpp>
#include <string>

struct id_t
{
    unsigned long long id;

    static auto comparator()
    {
        return [](const id_t& lhs, const id_t& rhs)
        {
            return lhs.id < rhs.id;
        };
    }
};

struct property_t;
struct attr_t
{
    std::string attr;

    template <typename T>
    property_t operator()(T&& value)
    {
        return { *this, value };
    }
};

struct property_t
{
    attr_t attr;
    boost::any value;
};


id_t operator "" _id(unsigned long long id)
{
    return id_t{ id };
}

attr_t operator "" _attr(const char* name, size_t sz)
{
    return attr_t{std::string(name)};
}

#if __cplusplus > 201703L
template <typename T>
auto operator<=>(const attr_t& attr, T&& value)
{
    return property_t{ attr, std::forward<T>(value) };
}
#endif
