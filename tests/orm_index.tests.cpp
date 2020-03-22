#include "3rdparty/boost/ut.hpp"
#include "include/orm_index.h"
#include "include/logger.h"

#include <boost/container/stable_vector.hpp>

struct MyStruct : bi_hook_type
{
    int64_t id;
    std::string title;
    std::string description;
    double timestamp;
};

struct id_indexer_t
{
    typedef decltype(MyStruct::id) type;

    const type& operator()(const MyStruct& v) const
    {
        return v.id;
    }
};

namespace orm_index_tests
{
    using namespace boost::ut;
    auto t1 = "orm_index_test1"_test = []()
    {
        boost::container::stable_vector<MyStruct> vec{ MyStruct{.id = 1}, MyStruct{.id = 2} };
        orm_index_t<MyStruct, id_indexer_t> index{};
        index.insert(*vec.begin());
        index.insert(*std::next(vec.begin()));

        vec.emplace_back(MyStruct{.id = 3});
        index.insert(vec.back());

        expect(index.size() == 3);
    };
}