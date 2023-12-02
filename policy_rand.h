// policy_rand.h
// Replacement Policy implementation in Random
// Author: Hank Bao

#pragma once

#include <vector>

#include "policy.h"

class ReplacementPolicyRand : public ReplacementPolicy {
   public:
    ReplacementPolicyRand(size_type capacity) : ReplacementPolicy{}, queue_{} {}
    virtual ~ReplacementPolicyRand() = default;

    virtual auto replace(
        std::map<size_type, TlbEntry>& cache, size_type cache_size, size_type vpn,
        size_type pfn, bool valid) -> std::optional<std::pair<size_type, TlbEntry>> override;

   private:
    std::vector<size_type> queue_;

   private:
    ReplacementPolicyRand(const ReplacementPolicyRand&) = delete;
    ReplacementPolicyRand& operator=(const ReplacementPolicyRand&) = delete;
};
