#include <doctest/doctest.h>

#include "include/orm_index.h"
#include "include/logger.h"
#include "include/orm_types.h"

#include <boost/container/stable_vector.hpp>
#include <tuple>

namespace
{
    struct Todo
    {
        const int64_t id;
        std::string title;
        std::string description;
        double timestamp;
    };

    using TodoSimple = orm_wrapper_t<Todo>;

    struct id_indexer_t
    {
        typedef decltype(TodoSimple::id) type;

        const type& operator()(const TodoSimple& v) const
        {
            return v.id;
        }
    };
}

#if 1
TEST_CASE("rehash_simple")
{
    boost::container::stable_vector<TodoSimple> vec{ TodoSimple{1}, TodoSimple{2} };
    orm_index_t<TodoSimple, id_indexer_t> index{};
    index.insert(*vec.begin());
    index.insert(*std::next(vec.begin()));

    vec.emplace_back(TodoSimple{3});
    index.insert(vec.back());

    CHECK(index.size() == 3);
};

TEST_CASE("find_simple")
{
    boost::container::stable_vector<TodoSimple> vec{ TodoSimple{1}, TodoSimple{2} };
    orm_index_t<TodoSimple, id_indexer_t> index{};
    index.insert(*vec.begin());
    index.insert(*std::next(vec.begin()));

    auto gen = index.find(1);
    auto it = gen.begin();
    CHECK(it != gen.end());
    CHECK((*it)->id == 1);
};

TEST_CASE("find_rehashing_aux_bucket")
{
    boost::container::stable_vector<TodoSimple> vec{
        TodoSimple{1},
        TodoSimple{2},
        TodoSimple{3}
    };

    orm_index_t<TodoSimple, id_indexer_t> index{};
    for (auto& v : vec)
        index.insert(v);

    // the 2-nd element should be in _index_aux
    auto gen = index.find(3);
    auto it = gen.begin();
    CHECK(it != gen.end());
    CHECK((*it)->id == 3);
    it++;
    CHECK(it == gen.end());
};

TEST_CASE("find_rehashing_main_bucket")
{
    boost::container::stable_vector<TodoSimple> vec{
        TodoSimple{1},
        TodoSimple{2},
        TodoSimple{3}
    };

    orm_index_t<TodoSimple, id_indexer_t> index{};
    for (auto& v : vec)
        index.insert(v);

    // the 1-nd element should be in _index
    auto gen = index.find(1);
    auto it = gen.begin();
    CHECK(it != gen.end());
    CHECK((*it)->id == 1);
    it++;
    CHECK(it == gen.end());
};

TEST_CASE("find_rehashing_both_buckets")
{
    boost::container::stable_vector<TodoSimple> vec{
        TodoSimple{1, "entry 1"},
        TodoSimple{0, "entry 2"},
        TodoSimple{1, "entry 3"},
    };

    orm_index_t<TodoSimple, id_indexer_t> index{};
    for (auto& v : vec)
        index.insert(v);

    // "entry 1" should be in main bucket
    // "entry 3" should be in aux bucket
    // generator should yield both
    auto gen = index.find(1);
    auto it = gen.begin();
    CHECK(it != gen.end());

    CHECK((*it)->id == 1);
    CHECK((*it)->title == "entry 1");
    it++;

    CHECK((*it)->id == 1);
    CHECK((*it)->title == "entry 3");
    it++;

    CHECK(it == gen.end());
};

TEST_CASE("update")
{
    boost::container::stable_vector<TodoSimple> vec{
        TodoSimple{1, "entry 1"},
        TodoSimple{2, "entry 2"},
        TodoSimple{3, "entry 3"},
    };

    orm_index_t<TodoSimple, id_indexer_t> index{};
    for (auto& v : vec)
        index.insert(v);

    auto it = index.find(1);
    auto& obj = *(*(it.begin()));
    obj.title = "modified entry 1";

    auto& obj_1 = *(*(index.find(1).begin()));   
    CHECK(obj_1.title == "modified entry 1");
}
#endif