#include <iostream>

#include "include/property.h"
#include "include/storage.h"
#include "include/struct_index.h"
#include "include/orm_index.h"
#include "include/logger.h"

int main()
{
    spdlog::set_level(spdlog::level::debug);

    const auto id = 10_id;
    const auto r = "id"_attr;

    const auto atp1 = "timestamp"_attr(double(17263.0));
    const auto atp2 = "title"_attr("some title");

    const auto index_vec = struct_index::get<Todo>();


    // 	storage_t<Todo> storage;
    // 	storage.insert(1_id, {"timestamp"_attr(double(12345)), "title"_attr(std::string("some titile"))});

    std::cout << "done" << std::endl;
    return 0;
}
