#pragma once

#include "property_storage.h"
#include "struct_orm.h"

#include <map>
#include <boost/container/stable_vector.hpp>

template <typename T>
struct storage_t
{
    storage_t()
    {
        auto add_index = [this]<typename U>(const std::string & field_name, auto & v)
        {
            property_storage_t<T, U> s{ {field_name} };
            //_indexes[field_name] = s;
        };
        struct_orm<T>::for_each(T{}, add_index);
    }

    error_t insert(id_t id, std::initializer_list<property_t> il)
    {
        // insert
        auto obj = struct_orm<T>::create_from(std::move(il));
        obj.id = id;
//         const auto [it, success] = _data.insert(decltype(_data)::value_type(id.id, obj));
// 
//         // update indexes
//         auto update_index = [this, &obj]<typename U>(const std::string & field_name, auto& v)
//         {
// 
//         };
//         struct_orm<T>::for_each(std::move(obj), update_index);
// 
//         return success ? error_t::success : error_t::failure;
    }


private:    
    boost::container::stable_vector<T> _data;

};