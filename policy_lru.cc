// policy_lru.cc
// Replacement Policy by LRU
// Author: Hank Bao

#include <cassert>
#include <utility>

#include "policy_lru.h"

auto ReplacementPolicyLru::replace(
    std::map<size_type, TlbEntry>& cache, size_type cache_size,
    size_type vpn, size_type pfn, bool valid) -> std::optional<std::pair<size_type, TlbEntry>> {
    // save pfn in the cache
    cache[vpn] = std::make_pair(pfn, valid);

    // save (vpn, pfn, valid) in the lru for bookkeeping and get the evictee simultaneously
    auto evictee = lru_.insert(vpn, std::make_pair(pfn, valid));
    if (evictee) {
        assert(cache.size() > cache_size);

        auto it = cache.find(evictee->first);
        assert(it != cache.end());

        cache.erase(it);

        return evictee;
    } else {
        return std::nullopt;
    }
}
