#include <doctest/doctest.h>

#include "include/orm_index.h"
#include "include/logger.h"

#include <boost/container/stable_vector.hpp>

struct Todo
{
    const int64_t id;
    std::string title;
    std::string description;
    double timestamp;
};

struct TodoSimple :  bi_hook_type
{
//    using Todo::Todo;
    const int64_t id;
    std::string title;
    std::string description;
    double timestamp;
};

struct id_indexer_t
{
    typedef decltype(TodoSimple::id) type;

    const type& operator()(const TodoSimple& v) const
    {
        return v.id;
    }
};


#if 1
TEST_CASE("rehash_simple")
{
    boost::container::stable_vector<TodoSimple> vec{ TodoSimple{.id = 1}, TodoSimple{.id = 2} };
    orm_index_t<TodoSimple, id_indexer_t> index{};
    index.insert(*vec.begin());
    index.insert(*std::next(vec.begin()));

    vec.emplace_back(TodoSimple{ .id = 3 });
    index.insert(vec.back());

    CHECK(index.size() == 3);
};

TEST_CASE("find_simple")
{
    boost::container::stable_vector<TodoSimple> vec{ TodoSimple{.id = 1}, TodoSimple{.id = 2} };
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
        TodoSimple{.id = 1},
        TodoSimple{.id = 2},
        TodoSimple{.id = 3}
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
        TodoSimple{.id = 1},
        TodoSimple{.id = 2},
        TodoSimple{.id = 3}
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
        TodoSimple{.id = 1, .description = "entry 1"},
        TodoSimple{.id = 0, .description = "entry 2"},
        TodoSimple{.id = 1, .description = "entry 3"},
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
    CHECK((*it)->description == "entry 1");
    it++;

    CHECK((*it)->id == 1);
    CHECK((*it)->description == "entry 3");
    it++;

    CHECK(it == gen.end());
};

TEST_CASE("update")
{
    boost::container::stable_vector<TodoSimple> vec{
        TodoSimple{.id = 1, .description = "entry 1"},
        TodoSimple{.id = 2, .description = "entry 2"},
        TodoSimple{.id = 3, .description = "entry 3"},
    };

    orm_index_t<TodoSimple, id_indexer_t> index{};
    for (auto& v : vec)
        index.insert(v);

    auto it = index.find(1);
    auto& obj = *(*(it.begin()));
    obj.description = "modified entry 1";

    auto& obj_1 = *(*(index.find(1).begin()));   
    CHECK(obj_1.description == "modified entry 1");
}
#endif