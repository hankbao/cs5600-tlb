// main.cc
// Simple tlb implementation for CS5600
// Author: Hank Bao

#include <memory>
#include <utility>
#include <vector>

#include <getopt.h>

#include "mmu.h"
#include "policy_fifo.h"
#include "policy_lru.h"
#include "policy_rand.h"
#include "tlb_impl.h"
#include "tlb_null.h"
#include "utils.h"

auto main(int argc, char** argv) -> int {
    uint32_t page_size = 4096;
    uint32_t tlb_size = 64;
    uint32_t tlb_cost = 5;
    uint32_t tlb_l2_size = 0;
    uint32_t tlb_l2_cost = 20;
    uint32_t pagetable_cost = 100;
    Policy tlb_policy = Policy::FIFO;
    Policy tlb_l2_policy = Policy::LRU;
    std::vector<uint32_t> access{};
    std::vector<uint32_t> prefetches{};

    int opt;
    struct option long_options[] = {
        {"size", optional_argument, nullptr, 's'},
        {"tlb", optional_argument, nullptr, 't'},
        {"cost", optional_argument, nullptr, 'c'},
        {"tlb2", optional_argument, nullptr, 'l'},
        {"cost2", optional_argument, nullptr, 'd'},
        {"costpt", optional_argument, nullptr, 'e'},
        {"policy", optional_argument, nullptr, 'p'},
        {"policy2", optional_argument, nullptr, 'q'},
        {"access", required_argument, nullptr, 'a'},
        {"prefetch", optional_argument, nullptr, 'f'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}};

    while ((opt = getopt_long(argc, argv, "s:t:c:l:d:e:p:q:a:f:h", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'h':
                print_usage(false);
                break;
            case 's':
                page_size = parse_page_size(optarg);
                break;
            case 't':
                tlb_size = parse_tlb_size(optarg);
                break;
            case 'c':
                tlb_cost = parse_cost(optarg);
                break;
            case 'l':
                tlb_l2_size = parse_tlb_size(optarg);
                break;
            case 'd':
                tlb_l2_cost = parse_cost(optarg);
                break;
            case 'e':
                pagetable_cost = parse_cost(optarg);
                break;
            case 'p':
                tlb_policy = parse_policy(optarg);
                break;
            case 'q':
                tlb_l2_policy = parse_policy(optarg);
                break;
            case 'a':
                access = parse_addrs(optarg);
                break;
            case 'f':
                prefetches = parse_addrs(optarg);
                break;
            default:
                std::fprintf(stderr, "Unrecognized option: %c\n", optopt);
                print_usage(true);
                break;
        }
    }

    std::printf("page_size: %u\n", page_size);
    std::printf("tlb_size: %u\n", tlb_size);
    std::printf("tlb_cost: %u\n", tlb_cost);
    std::printf("tlb_l2_size: %u\n", tlb_l2_size);
    std::printf("tlb_l2_cost: %u\n", tlb_l2_cost);
    std::printf("pagetable_cost: %u\n", pagetable_cost);
    std::printf("tlb_policy: %s\n", policy_to_string(tlb_policy).c_str());
    std::printf("tlb_l2_policy: %s\n", policy_to_string(tlb_l2_policy).c_str());
    std::printf("access: %s\n", addrs_to_string(access).c_str());
    std::printf("prefetch: %s\n", addrs_to_string(prefetches).c_str());
    std::puts("");

    std::unique_ptr<Tlb> tlb_null = std::make_unique<TlbNull>();
    std::unique_ptr<Tlb> tlb_l2 = nullptr;
    std::unique_ptr<Tlb> tlb = nullptr;

    switch (tlb_l2_policy) {
        case Policy::FIFO:
            tlb_l2 = std::make_unique<TlbImpl<ReplacementPolicyFifo>>(tlb_l2_cost, tlb_l2_size, std::move(tlb_null));
            break;

        case Policy::LRU:
            tlb_l2 = std::make_unique<TlbImpl<ReplacementPolicyLru>>(tlb_l2_cost, tlb_l2_size, std::move(tlb_null));
            break;

        case Policy::Random:
            tlb_l2 = std::make_unique<TlbImpl<ReplacementPolicyRand>>(tlb_l2_cost, tlb_l2_size, std::move(tlb_null));
            break;

        default:
            std::abort();
    }

    switch (tlb_policy) {
        case Policy::FIFO:
            tlb = std::make_unique<TlbImpl<ReplacementPolicyFifo>>(tlb_cost, tlb_size, std::move(tlb_l2));
            break;

        case Policy::LRU:
            tlb = std::make_unique<TlbImpl<ReplacementPolicyLru>>(tlb_cost, tlb_size, std::move(tlb_l2));
            break;

        case Policy::Random:
            tlb = std::make_unique<TlbImpl<ReplacementPolicyRand>>(tlb_cost, tlb_size, std::move(tlb_l2));
            break;

        default:
            std::abort();
    }

    auto mmu = std::make_unique<Mmu>(std::move(tlb), pagetable_cost, page_size);
    if (!prefetches.empty()) {
        for (const auto& addr : prefetches) {
            mmu->access(addr, true);
        }
    }

    uint32_t hits = 0;
    uint32_t misses = 0;
    uint32_t total_cost = 0;

    for (const auto& addr : access) {
        auto result = mmu->access(addr, false);
        if (result.first) {
            hits += 1;
        } else {
            misses += 1;
        }
        total_cost += result.second;
    }

    std::printf("\nFINALSTATS hits %d, misses %d, hitrate %.2f, total cost %uns, average cost %.2fns\n",
                hits, misses, hits / (float)(hits + misses), total_cost, total_cost / (float)(hits + misses));

    return EXIT_SUCCESS;
}
