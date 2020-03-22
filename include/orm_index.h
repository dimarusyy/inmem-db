#pragma once

#include <boost/intrusive/unordered_set.hpp>
#include "logger.h"
#include "error.h"

namespace bi = boost::intrusive;

using bi_hook_type = bi::unordered_set_base_hook<
    bi::link_mode<bi::safe_link>,
    bi::optimize_multikey<true>,
    bi::compare_hash<true>,
    bi::incremental<true>>;

template <typename T, typename Indexer>
struct orm_index_t  // pow^2 of bucket size
{
    using container_type = bi::unordered_multiset<T, bi::key_of_value<Indexer>>;
    using bucket_type = typename container_type::bucket_type;

    orm_index_t(std::size_t sz = 2)
        : _buckets(sz)
        , _index(container_type::bucket_traits(_buckets.data(), sz))
        , _buckets_aux(1)
        , _index_aux(container_type::bucket_traits(_buckets_aux.data(), 1))
    {
        assert((sz & (sz - 1)) == 0);
    }
    template <typename Iterator>
    orm_index_t(Iterator begin, Iterator end, std::size_t sz = 2)
        : _buckets(sz)
        , _index(begin, end, container_type::bucket_traits(_buckets.data(), sz))
        , _buckets_aux(1)
        , _index_aux(container_type::bucket_traits(_buckets_aux.data(), 1))
    {
        assert((sz & (sz - 1)) == 0);
    }

    auto insert(typename container_type::reference& entry)
    {
        return try_rehash() ? _index_aux.insert(entry) : _index.insert(entry);
    }

    auto size() const
    {
        return _index.size();
    }

protected:
    // returns true if rehashing
    virtual bool try_rehash()
    {
        if (_index.size() >= _buckets.size() && !_is_rehashing)
        {
            _is_rehashing = true;

            // alloc aux bucket
            const auto new_sz = _buckets.size() << 1;
            auto tmp_bucket = decltype(_buckets_aux)(new_sz);
            _index_aux.rehash(container_type::bucket_traits(tmp_bucket.data(), new_sz));
            _buckets_aux = std::move(tmp_bucket);
        }

        if (!_is_rehashing)
        {
            return false;
        }
        else if (_is_rehashing && _index.size() == 0)
        {
            _index.swap(_index_aux);
            _buckets.swap(_buckets_aux);
            _buckets_aux.clear();
            _is_rehashing = false;

            return false;
        }

        // move 1 element
        auto it = _index.begin();
        auto& obj = *it;
        
        _index.erase(it);
        _index_aux.insert(obj);

        spdlog::debug("main index size=[{0}], aux index size=[{1}]", _index.size(), _index_aux.size());

        return true;
    }

private:
    std::vector<bucket_type> _buckets;
    container_type _index;

    std::vector<bucket_type> _buckets_aux;
    container_type _index_aux;

    bool _is_rehashing{ false };
};