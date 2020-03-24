#pragma once

#include <boost/container/flat_map.hpp>
#include <boost/fusion/include/define_struct_inline.hpp>
#include <utility>

struct Todo
{
    const int64_t id;
    std::string title;
    std::string description;
    double timestamp;
};

BOOST_FUSION_ADAPT_STRUCT(
    Todo,
    (auto, id)
    (auto, title)
    (auto, description)
    (auto, timestamp)
)
