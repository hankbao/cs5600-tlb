// tlb_null.h
// TLB null implementation
// Author: Hank Bao

#pragma once

#include "def.h"
#include "tlb.h"

class TlbNull : public Tlb {
   public:
    TlbNull() = default;
    virtual ~TlbNull() = default;

    virtual auto lookup(size_type vpn) -> std::optional<std::pair<size_type, time_type>> override { return std::nullopt; }
    virtual auto insert(size_type vpn, size_type pfn, bool valid) -> void override {}

   private:
    TlbNull(const TlbNull&) = delete;
    TlbNull& operator=(const TlbNull&) = delete;
};
