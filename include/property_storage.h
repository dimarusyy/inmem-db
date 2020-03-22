#pragma once

#include "config.h"
#include "property.h"
#include "error.h"

#include <boost/container/stable_vector.hpp>
#include <boost/container/flat_set.hpp>

template <typename T, typename P, typename Holder = default_holder_t<T,P>>
struct property_storage_t : protected Holder
{
    using container_t = Holder;

    property_storage_t(attr_t attr)
        : _attr(std::move(attr))
    {
    }
    
    attr_t attr() const
    {
        return _attr;
    }

    error_t insert(P&& key, T&& data)
    {
        const auto it_deleted = _deleted.find(std::forward<P>(key));
        if ( it_deleted != end(_deleted))
        {
            // deleted key found
            // unmark as deleted
            _deleted.erase(it_deleted);
        }
        const auto rc = _data.try_emplace(std::forward<P>(key), std::ref(std::forward<T>(data)));
        if (!rc.second)
            return error_t::failure;
        return error_t::success;
    }

    error_t update(P&& key, T&& data)
    {
        const auto rc = _data.insert_or_assign(std::forward<P>(key), std::ref(std::forward<T>(data)));
        return error_t::success;
    }

    error_t erase(P&& key)
    {
        _deleted.insert(key);
    }

    typename container_t::iterator get(P&& key) const
    {
        if (_deleted.find(std::forward<P>(key)) != end(_deleted))
            return _data.end();
        return { _data.find(std::forward<P>(key)) };
    }

protected:
    virtual bool should_rehash() const noexcept
    {
        return (_data.size() / _deleted.size()) > 10;
    }

    virtual void update()
    {
        if (!should_rehash())
            return;
        // todo
    }

private:
    attr_t _attr;
    Holder _data;
    boost::container::flat_set<attr_t> _deleted;

    bool _is_rehashing{ false };
};