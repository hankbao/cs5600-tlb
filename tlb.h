// tlb.h
// TLB interface definition
// Author: Hank Bao

#pragma once

#include <optional>
#include <utility>

#include "def.h"

class Tlb {
   public:
    Tlb() = default;
    virtual ~Tlb() = default;

    virtual auto lookup(size_type vpn) -> std::optional<std::pair<size_type, time_type>> = 0;
    virtual auto insert(size_type vpn, size_type pfn, bool valid) -> void = 0;

   private:
    Tlb(const Tlb&) = delete;
    Tlb& operator=(const Tlb&) = delete;
};
