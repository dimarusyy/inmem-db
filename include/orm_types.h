#pragma once

#include <boost/intrusive/unordered_set.hpp>
#include <tuple>

namespace bi = boost::intrusive;

using bi_hook_type = bi::unordered_set_base_hook<
    bi::link_mode<bi::safe_link>,
    bi::optimize_multikey<true>,
    bi::compare_hash<true>,
    bi::incremental<true>>;

template <typename T>
struct orm_wrapper_t : T, bi_hook_type
{
    template <class... Us>
    orm_wrapper_t(Us const&... us)
        : orm_wrapper_t(std::tuple_cat(std::forward_as_tuple(us)...))
    {
    }

    template <class... Us>
    orm_wrapper_t(std::tuple<Us...> const& t)
        : orm_wrapper_t(t, std::index_sequence_for<Us...>{})
    {
    }

    template <class... Us, size_t... Is>
    orm_wrapper_t(std::tuple<Us...> const& t, std::index_sequence<Is...>)
        : T{ std::get<Is>(t)... }
    {
    }
};
