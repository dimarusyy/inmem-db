#pragma once

#include "property.h"

#include <boost/mpl/range_c.hpp>
#include <boost/fusion/adapted/struct/define_assoc_struct.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/algorithm/transformation/zip.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/algorithm/transformation/transform.hpp>

template <typename T>
struct struct_orm
{
    template <typename Handler>
    static void for_each(T&& v, Handler handler)
    {
        namespace fusion = boost::fusion;
        namespace mpl = boost::mpl;

        typedef mpl::range_c<unsigned, 0, fusion::result_of::size<T>::type::value> range;
        fusion::for_each(range(), [&](auto index)
                         {
                             const auto field_name = fusion::extension::struct_member_name<T, index>::call();
                             auto& field_value_ref = fusion::at_c<index>(v);
                             typedef std::decay_t<decltype(field_value_ref)> field_type;

                             handler.template operator()<field_type>(field_name, field_value_ref);
                         });
    }


    static T create_from(std::initializer_list<property_t> il)
    {
        T obj{};
        for (auto p : il)
        {
            auto fill_in = [&p]<typename U>(auto & field_name, auto & v)
            {
                if (p.attr.attr == field_name)
                {
                    v = boost::any_cast<U>(p.value);
                }
            };
            struct_orm<T>::for_each(std::move(obj), fill_in);
        }
        return obj;
    }

};