// main.cc
// Simple tlb implementation for CS5600
// Author: Hank Bao

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <getopt.h>

#include "mmu.h"
#include "policy_fifo.h"
#include "policy_lru.h"
#include "policy_rand.h"
#include "tlb_impl.h"
#include "tlb_null.h"

enum class Policy {
    FIFO,
    LRU,
    Random,
};

[[noreturn]] auto print_usage(bool onerror) -> void {
    std::puts("Usage: tlb [OPTIONS]...\n");
    std::puts("Supported options:");
    std::puts("-s, --size=PAGESIZE\n\tsize of a page in bytes, must be a power of 2");
    std::puts("-t, --tlb=TLBSIZE\n\tsize of the TLB L1");
    std::puts("-c, --cost=TLBCOST\n\tcost of lookup in the TLB L1, default to 5 nano seconds");
    std::puts("-l, --tlb2=TLBSIZE2\n\tsize of the TLB L2");
    std::puts("-d, --cost2=TLBCOST2\n\tcost of lookup in the TLB L2, default to 20 nano seconds");
    std::puts("-e, --costpt=PTBCOST\n\tcost of lookup in the Page Table, default to 100 nano seconds");
    std::puts("-p, --policy=TLBPOLICY\n\treplacement policy for TLB L1 (FIFO, LRU, RAND)");
    std::puts("-q, --policy2=TLBPOLICY2\n\treplacement policy for TLB L2 (FIFO, LRU, RAND)");
    std::puts("-a, --access=ADDRLIST\n\ta set of comma-separated addresses to access");
    std::puts("-f, --prefetch=PREFETCHLIST\n\ta set of comma-separated addresses to prefetch");
    std::puts("-h, --help\n\tprint usage message and exit");

    ::exit(onerror ? EXIT_FAILURE : EXIT_SUCCESS);
}

auto split_string(const std::string& s, const std::string& delimiter) -> std::vector<std::string> {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos) {
        result.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }

    result.push_back(s.substr(start, end));
    return result;
}

auto policy_to_string(const Policy& policy) -> std::string {
    switch (policy) {
        case Policy::FIFO:
            return "FIFO";
        case Policy::LRU:
            return "LRU";
        case Policy::Random:
            return "RAND";
    }
}

auto parse_page_size(const std::string& str) -> uint32_t {
    size_t size = std::stoi(str);
    if (size <= 0 || (size & (size - 1)) != 0) {
        std::fprintf(stderr, "Invalid page size: %s\n", str.c_str());
        print_usage(true);
    }

    return size;
}

auto parse_tlb_size(const std::string& str) -> uint32_t {
    size_t size = std::stoi(str);
    if (size <= 0) {
        std::fprintf(stderr, "Invalid tlb size: %s\n", str.c_str());
        print_usage(true);
    }

    return size;
}

auto parse_cost(const std::string& str) -> uint32_t {
    size_t cost = std::stoi(str);
    if (cost <= 0) {
        std::fprintf(stderr, "Invalid cost: %s\n", str.c_str());
        print_usage(true);
    }

    return cost;
}

auto parse_policy(const std::string& policy) -> Policy {
    if (policy == "FIFO") {
        return Policy::FIFO;
    } else if (policy == "LRU") {
        return Policy::LRU;
    } else if (policy == "RAND") {
        return Policy::Random;
    } else {
        std::fprintf(stderr, "Invalid policy: %s\n", policy.c_str());
        print_usage(true);
    }
}

auto parse_addrs(const std::string& addrs) -> std::vector<size_t> {
    auto addresses = std::vector<size_t>{};

    auto strlist = split_string(addrs, ",");
    for (const auto& str : strlist) {
        if (str.empty()) {
            std::fprintf(stderr, "Invalid address: <empty>. Skipped.\n");
            continue;
        } else {
            size_t addr = std::stoi(str);
            if (addr <= 0) {
                std::fprintf(stderr, "Invalid address: %s\n", str.c_str());
                print_usage(true);
            } else {
                addresses.push_back(addr);
            }
        }
    }

    return addresses;
}

auto addrs_to_string(const std::vector<size_t>& addrs) -> std::string {
    if (addrs.empty()) {
        return "<empty>";
    } else {
        std::string result;
        for (const auto& addr : addrs) {
            result += std::to_string(addr) + ",";
        }

        return result.substr(0, result.length() - 1);
    }
}

auto main(int argc, char** argv) -> int {
    uint32_t page_size = 4096;
    uint32_t tlb_size = 64;
    uint32_t tlb_cost = 5;
    uint32_t tlb_l2_size = 0;
    uint32_t tlb_l2_cost = 20;
    uint32_t pagetable_cost = 100;
    Policy tlb_policy = Policy::FIFO;
    Policy tlb_l2_policy = Policy::LRU;
    std::vector<size_t> access{};
    std::vector<size_t> prefetches{};

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

    // TODO: implement the TLB simulation
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
            mmu->access(addr);
        }
    }

    for (const auto& addr : access) {
        mmu->access(addr);
    }

    return EXIT_SUCCESS;
}
