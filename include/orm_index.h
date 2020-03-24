#pragma once

#include "logger.h"

#include <experimental/coroutine>

#include <cppcoro/generator.hpp>
#pragma comment(lib, "cppcoro.lib")

#include "orm_types.h"

template <typename T, typename Indexer>
struct orm_index_t  // pow^2 of bucket size
{
    using container_type = bi::unordered_multiset<T, bi::key_of_value<Indexer>>;
    using bucket_type = typename container_type::bucket_type;

    orm_index_t(std::size_t sz = 2)
        : _buckets(sz)
        , _index(container_type::bucket_traits(_buckets.data(), sz))
    {
        assert((sz & (sz - 1)) == 0);
    }

    template <typename Iterator>
    orm_index_t(Iterator begin, Iterator end, std::size_t sz = 2)
        : _buckets(sz)
        , _index(begin, end, container_type::bucket_traits(_buckets.data(), sz))
    {
        assert((sz & (sz - 1)) == 0);
    }

    auto insert(typename container_type::reference& entry)
    {
        return try_rehash() ? _index_aux.insert(entry) : _index.insert(entry);
    }

    auto erase(typename container_type::iterator it)
    {
        try_rehash();
        if(_is_rehashing)
            _index_aux.erase(it);
        _index.erase(it);
    }

    cppcoro::generator<typename container_type::iterator> 
        find(const typename container_type::key_type& key)        
    {
        const auto range = _index.equal_range(key);
        for(auto it = range.first; it != range.second; it++)
            co_yield it;
        if (_is_rehashing)
        {
            // try aux bucket
            const auto range_aux = _index_aux.equal_range(key);
            for (auto it = range_aux.first; it != range_aux.second; it++)
                co_yield it;
        }
    }

    auto size() const
    {
        return _is_rehashing ? _index.size() + _index_aux.size() : _index.size();
    }

protected:
    // returns true if rehashing
    virtual bool try_rehash()
    {
        if (_index.size() >= _buckets.size() && !_is_rehashing)
        {
            spdlog::info("initiating rehashing, index size=[{0}]", _index.size());
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

            spdlog::info("rehashing succeeded");
            return false;
        }

        // move 1 element
        // TBD : consider decreasing bucket size by rehashing to bucket with less size
        auto it = _index.begin();
        auto& obj = *it;
        
        _index.erase(it);
        _index_aux.insert(obj);

        spdlog::debug("rehashed 1-st enrty, main index size=[{0}], aux index size=[{1}]", _index.size(), _index_aux.size());

        return true;
    }

private:
    std::vector<bucket_type> _buckets;
    container_type _index;

    std::vector<bucket_type> _buckets_aux{ 1 };
    container_type _index_aux{ typename container_type::bucket_traits(_buckets_aux.data(), 1) };

    bool _is_rehashing{ false };
};