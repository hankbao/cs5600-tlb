// policy_fifo.h
// Replacement Policy implementation as FIFO
// Author: Hank Bao

#pragma once

#include <vector>

#include "policy.h"

class ReplacementPolicyFifo : public ReplacementPolicy {
   public:
    ReplacementPolicyFifo(size_type capacity) : ReplacementPolicy{}, queue_{} {}
    virtual ~ReplacementPolicyFifo() = default;

    virtual auto replace(
        std::map<size_type, TlbEntry>& cache, size_type cache_size, size_type vpn,
        size_type pfn, bool valid) -> std::optional<std::pair<size_type, TlbEntry>> override;

   private:
    std::vector<size_type> queue_;

   private:
    ReplacementPolicyFifo(const ReplacementPolicyFifo&) = delete;
    ReplacementPolicyFifo& operator=(const ReplacementPolicyFifo&) = delete;
};
