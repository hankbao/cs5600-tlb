// policy_lru.h
// Replacement Policy implementation as LRU
// Author: Hank Bao

#pragma once

#include "policy.h"
#include "lru_cache.h"

class ReplacementPolicyLru : public ReplacementPolicy {
   public:
    ReplacementPolicyLru(size_type capacity) : ReplacementPolicy{}, lru_{capacity} {}
    virtual ~ReplacementPolicyLru() = default;

    virtual auto replace(
        std::map<size_type, TlbEntry>& cache, size_type cache_size, size_type vpn,
        size_type pfn, bool valid) -> std::optional<std::pair<size_type, TlbEntry>> override;

   private:
    lru_cache<size_type, TlbEntry> lru_;

   private:
    ReplacementPolicyLru(const ReplacementPolicyLru&) = delete;
    ReplacementPolicyLru& operator=(const ReplacementPolicyLru&) = delete;
};
