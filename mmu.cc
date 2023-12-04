// mmu.cc
// MMU simulator implementation
// Author: Hank Bao

#include "mmu.h"

auto Mmu::access(addr_type vaddr, bool prefetching) -> std::pair<addr_type, time_type> {
    auto vpn = get_vpn(vaddr);
    auto offset = get_offset(vaddr);

    auto attempt = access_tlb(vpn);
    auto result = attempt.value_or(access_pagetable(vpn));

    auto pfn = result.first;
    auto cost = result.second;
    auto paddr = (pfn << offset_mask_) | offset;

    if (!attempt.has_value()) {
        tlb_->insert(vpn, pfn, true);
    }

    return std::make_pair(paddr, cost);
}

auto Mmu::access_tlb(addr_type vaddr) -> std::optional<std::pair<addr_type, time_type>> {
    return tlb_->lookup(get_vpn(vaddr));
}

auto Mmu::access_pagetable(addr_type vaddr) -> std::pair<addr_type, time_type> {
    auto vpn = get_vpn(vaddr);
    auto offset = get_offset(vaddr);

    auto pfn = fake_pagetable_map(vpn);
    auto paddr = (pfn << offset_mask_) | offset;

    return std::make_pair(paddr, pagetable_cost_);
}

auto Mmu::get_vpn(addr_type vaddr) -> size_type {
    return (vaddr & ~offset_mask_) >> offset_bits_;
}

auto Mmu::get_offset(addr_type vaddr) -> size_type {
    return vaddr & offset_mask_;
}

// map a virtual page number to a physical frame number
auto Mmu::fake_pagetable_map(addr_type vpn) -> size_type {
    return vpn + 0x2000;
}
