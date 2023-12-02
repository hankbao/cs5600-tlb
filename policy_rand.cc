// policy_rand.cc
// Replacement Policy by Random
// Author: Hank Bao

#include <cassert>
#include <random>
#include <utility>

#include "policy_rand.h"

auto ReplacementPolicyRand::replace(
    std::map<size_type, TlbEntry>& cache, size_type cache_size, size_type vpn,
    size_type pfn, bool valid) -> std::optional<std::pair<size_type, TlbEntry>> {
    // save pfn in the cache
    cache[vpn] = std::make_pair(pfn, valid);

    // save vpn in the queue for bookkeeping
    queue_.push_back(vpn);

    // evict one item randomly if the cache is full
    if (cache.size() > cache_size) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, cache_size - 1);

        auto idx = distrib(gen);
        auto victim = queue_.at(idx);
        queue_.erase(queue_.begin() + idx);

        auto it = cache.find(victim);
        assert(it != cache.end());

        auto kv = *it;
        cache.erase(it);

        return kv;
    } else {
        return std::nullopt;
    }
}
