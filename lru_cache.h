// lru_cache.h
// a cache which evicts the least recently used item when it is full
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.

#include <list>
#include <map>
#include <optional>
#include <utility>

template <class Key, class Value>
class lru_cache {
   public:
    typedef Key key_type;
    typedef Value value_type;
    typedef std::list<key_type> list_type;
    typedef std::map<key_type, std::pair<value_type, typename list_type::iterator>> map_type;

    lru_cache(size_t capacity) noexcept : m_map{}, m_list{}, m_capacity{capacity} {}

    ~lru_cache() {}

    size_t size() const noexcept { return m_map.size(); }

    size_t capacity() const noexcept { return m_capacity; }

    bool empty() const noexcept { return m_map.empty(); }

    bool contains(const key_type& key) noexcept { return m_map.find(key) != m_map.end(); }

    std::optional<std::pair<key_type, value_type>> insert(const key_type& key, const value_type& value) noexcept {
        std::optional<std::pair<key_type, value_type>> evictee = std::nullopt;

        typename map_type::iterator i = m_map.find(key);
        if (i == m_map.end()) {
            // insert item into the cache, but first check if it is full
            if (size() >= m_capacity) {
                // cache is full, evict the least recently used item
                evictee = evict();
            }

            // insert the new item
            m_list.push_front(key);
            m_map[key] = std::make_pair(value, m_list.begin());
        }

        return evictee;
    }

    std::optional<value_type> get(const key_type& key) noexcept {
        // lookup value in the cache
        typename map_type::iterator i = m_map.find(key);
        if (i == m_map.end()) {
            // value not in cache
            return std::nullopt;
        }

        // return the value, but first update its place in the most
        // recently used list
        typename list_type::iterator j = i->second.second;
        if (j != m_list.begin()) {
            // move item to the front of the most recently used list
            m_list.erase(j);
            m_list.push_front(key);

            // update iterator in map
            j = m_list.begin();
            const value_type& value = i->second.first;
            m_map[key] = std::make_pair(value, j);

            // return the value
            return value;
        } else {
            // the item is already at the front of the most recently
            // used list so just return it
            return i->second.first;
        }
    }

    void clear() noexcept {
        m_map.clear();
        m_list.clear();
    }

   private:
    std::pair<key_type, value_type> evict() noexcept {
        // evict item from the end of most recently used list
        typename list_type::iterator i = --m_list.end();
        auto v = m_map.at(*i);

        m_map.erase(*i);
        m_list.erase(i);

        return std::make_pair(*i, v.first);
    }

   private:
    map_type m_map;
    list_type m_list;
    size_t m_capacity;
};
