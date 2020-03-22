#pragma once

#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/mpl/range_c.hpp>

#include <map>
#include <string>

namespace struct_index
{
    template <typename T>
    std::map<std::string, int> get()
    {
        namespace fusion = boost::fusion;
        namespace mpl = boost::mpl;

        std::map<std::string, int> rc;
        fusion::for_each(mpl::range_c<unsigned, 0, fusion::result_of::size<T>::value>(),
                         [&](auto index)
                         {
                             rc.insert({ fusion::extension::struct_member_name<T, index>::call(), index });
                         });
        return rc;
    }
}