#pragma once

#include <boost/container/flat_map.hpp>
#include <boost/fusion/include/define_struct_inline.hpp>
#include <utility>

// TBD : 
// 1) consider using google.btree
// 2) consider using intrusive.hashtable + std::array<>

template <typename T, typename P>
using default_holder_t = boost::container::flat_multimap<P, std::reference_wrapper<T>>;

// should be here ?
struct Todo
{
    int64_t id;
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
