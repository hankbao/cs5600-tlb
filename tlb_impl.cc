// tlb_impl.cc
// TLB implementation
// Author: Hank Bao

#include <cassert>

#include "tlb_impl.h"
#include "policy_fifo.h"
#include "policy_lru.h"
#include "policy_rand.h"

template <typename RP>
auto TlbImpl<RP>::lookup(size_type vpn) -> std::optional<std::pair<size_type, time_type>> {
    // lookup in current level
    const auto it = cache_.find(vpn);

    // search cache
    if (it != cache_.end()) {
        // tlb hit
        return std::pair(it->second.first, cost_);
    } else {
        // tlb miss, try next level
        return next_->lookup(vpn);
    }
}

template <typename RP>
auto TlbImpl<RP>::insert(size_type vpn, size_type pfn, bool valid) -> void {
    auto entry = RP::replace(cache_, capacity_, vpn, pfn, valid);
    if (entry) {
        assert(cache_.size() == capacity_);

        auto vpn = entry->first;
        auto pfn = entry->second.first;
        auto valid = entry->second.second;

        // put the evictee one into next level
        next_->insert(vpn, pfn, valid);
    } else {
        assert(cache_.size() < capacity_);
    }
}

template auto TlbImpl<ReplacementPolicyFifo>::lookup(size_type vpn) -> std::optional<std::pair<size_type, time_type>>;
template auto TlbImpl<ReplacementPolicyLru>::lookup(size_type vpn) -> std::optional<std::pair<size_type, time_type>>;
template auto TlbImpl<ReplacementPolicyRand>::lookup(size_type vpn) -> std::optional<std::pair<size_type, time_type>>;

template auto TlbImpl<ReplacementPolicyFifo>::insert(size_type vpn, size_type pfn, bool valid) -> void;
template auto TlbImpl<ReplacementPolicyLru>::insert(size_type vpn, size_type pfn, bool valid) -> void;
template auto TlbImpl<ReplacementPolicyRand>::insert(size_type vpn, size_type pfn, bool valid) -> void;
