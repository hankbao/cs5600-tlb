// tlb_impl.h
// TLB implementation
// Author: Hank Bao

#pragma once

#include <map>
#include <memory>
#include <utility>

#include "def.h"
#include "policy.h"
#include "tlb.h"

template <typename RP>
class TlbImpl : public Tlb, private RP {
   public:
    TlbImpl(const time_type cost, const size_type capacity, std::unique_ptr<Tlb>&& next)
        : Tlb{},
          RP{capacity},
          cost_{cost},
          capacity_{capacity},
          cache_{},
          next_{std::forward<decltype(next)>(next)} {}
    virtual ~TlbImpl() = default;

    virtual auto lookup(size_type vpn) -> std::optional<std::pair<size_type, time_type>> override;
    virtual auto insert(size_type vpn, size_type pfn, bool valid) -> void override;

   private:
    const time_type cost_;
    const size_t capacity_;
    std::map<size_type, TlbEntry> cache_;
    std::unique_ptr<Tlb> next_;

    TlbImpl(const TlbImpl&) = delete;
    TlbImpl& operator=(const TlbImpl&) = delete;
};
