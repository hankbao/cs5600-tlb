// mmu.h
// MMU simulator implementation
// Author: Hank Bao

#pragma once

#include <cmath>
#include <memory>
#include <optional>
#include <utility>

#include "def.h"
#include "tlb.h"

class Mmu {
   public:
    Mmu(std::unique_ptr<Tlb>&& tlb, time_type pagetable_cost, size_type page_size)
        : tlb_{std::forward<decltype(tlb)>(tlb)},
          pagetable_cost_{pagetable_cost},
          offset_mask_{page_size - 1},
          offset_bits_{static_cast<size_type>(std::log2(page_size))} {}
    ~Mmu() = default;

    auto access(addr_type vaddr, bool prefetching) -> std::pair<bool, time_type>;

   private:
    auto access_tlb(size_type vpn) -> std::optional<std::pair<addr_type, time_type>>;
    auto access_pagetable(size_type vpn) -> std::pair<addr_type, time_type>;

    auto get_vpn(addr_type vaddr) -> size_type;
    auto get_offset(addr_type vaddr) -> size_type;
    auto fake_pagetable_map(size_type vpn) -> size_type;

   private:
    std::unique_ptr<Tlb> tlb_;
    const time_type pagetable_cost_;
    const addr_type offset_mask_;
    const size_type offset_bits_;

   private:
    Mmu(const Mmu&) = delete;
    Mmu& operator=(const Mmu&) = delete;
};
