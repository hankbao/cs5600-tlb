// policy_fifo.cc
// Replacement Policy by FIFO
// Author: Hank Bao

#include <cassert>
#include <utility>

#include "policy_fifo.h"

auto ReplacementPolicyFifo::replace(
    std::map<size_type, TlbEntry>& cache, size_type cache_size,
    size_type vpn, size_type pfn, bool valid) -> std::optional<std::pair<size_type, TlbEntry>> {
    // save pfn in the cache
    cache[vpn] = std::make_pair(pfn, valid);

    // save vpn in the queue for bookkeeping
    queue_.push_back(vpn);

    // evict the first vpn if the cache is full
    if (cache.size() > cache_size) {
        auto front = queue_.front();
        queue_.erase(queue_.begin());

        auto it = cache.find(front);
        assert(it != cache.end());

        auto kv = *it;
        cache.erase(it);

        return kv;
    } else {
        return std::nullopt;
    }
}
