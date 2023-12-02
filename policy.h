// policy.h
// Replacement Policy interface definition
// Author: Hank Bao

#pragma once

#include <map>
#include <optional>

#include "def.h"

class ReplacementPolicy {
   public:
    ReplacementPolicy() = default;
    virtual ~ReplacementPolicy() = default;

    virtual auto replace(
        std::map<size_type, TlbEntry>& cache, size_type cache_size, size_type vpn,
        size_type pfn, bool valid) -> std::optional<std::pair<size_type, TlbEntry>> = 0;

   private:
    ReplacementPolicy(const ReplacementPolicy&) = delete;
    ReplacementPolicy& operator=(const ReplacementPolicy&) = delete;
};
