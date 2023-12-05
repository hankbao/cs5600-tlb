// mmu.cc
// MMU simulator implementation
// Author: Hank Bao

#include <cstdio>

#include "mmu.h"

auto Mmu::access(addr_type vaddr, bool prefetching) -> std::pair<bool, time_type> {
    auto vpn = get_vpn(vaddr);
    auto offset = get_offset(vaddr);

    auto attempt = access_tlb(vpn);
    auto result = attempt.value_or(access_pagetable(vpn));

    auto pfn = result.first;
    auto cost = result.second;
    auto paddr = (pfn << offset_mask_) | offset;

    bool hit = true;
    if (!attempt.has_value()) {
        hit = false;
        tlb_->insert(vpn, pfn, true);
    }

    if (!prefetching) {
        std::printf("MMU access: %s, VADDR=0x%08x, VPN=%u, OFFSET=0x%08x, PFN=%u, PADDR=0x%08x COST=%uns\n",
                    hit ? "HIT" : "MISS", vaddr, vpn, offset, pfn, paddr, cost);
    } else {
        std::printf("TLB prefetch: %s, VADDR=0x%08x, VPN=%u, PFN=%u\n",
                    hit ? "HIT" : "MISS", vaddr, vpn, pfn);
    }

    return std::make_pair(hit, cost);
}

auto Mmu::access_tlb(size_type vpn) -> std::optional<std::pair<addr_type, time_type>> {
    return tlb_->lookup(vpn);
}

auto Mmu::access_pagetable(size_type vpn) -> std::pair<addr_type, time_type> {
    auto pfn = fake_pagetable_map(vpn);
    return std::make_pair(pfn, pagetable_cost_);
}

auto Mmu::get_vpn(addr_type vaddr) -> size_type {
    return (vaddr & ~offset_mask_) >> offset_bits_;
}

auto Mmu::get_offset(addr_type vaddr) -> size_type {
    return vaddr & offset_mask_;
}

// map a virtual page number to a physical frame number
auto Mmu::fake_pagetable_map(size_type vpn) -> size_type {
    return vpn + 0x2000;
}
