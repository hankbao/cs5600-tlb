// utils.cc
// Utilities to handle I/O and display with conversion support.
// Author: Hank Bao

#include "utils.h"

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

auto begins_with(const std::string& s, const std::string& prefix) -> bool {
    return s.length() >= prefix.length() ? s.compare(0, prefix.length(), prefix) == 0 : false;
}

auto str_to_num(const std::string& s) -> uint32_t {
    assert(!s.empty());

    int base = 10;
    if (begins_with(s, "0x") || begins_with(s, "0X")) {
        base = 16;
    } else if (begins_with(s, "0b") || begins_with(s, "0B")) {
        base = 2;
    } else if (begins_with(s, "0")) {
        base = 8;
    }

    return std::strtoul(s.c_str(), nullptr, base);
}

auto num_to_str_hex(uint32_t n) -> std::string {
    std::stringstream ss;
    ss << "0x" << std::hex << n;
    return ss.str();
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
    uint32_t size = str_to_num(str);
    if (size <= 0 || (size & (size - 1)) != 0) {
        std::fprintf(stderr, "Invalid page size: %s\n", str.c_str());
        print_usage(true);
    }

    return size;
}

auto parse_tlb_size(const std::string& str) -> uint32_t {
    uint32_t size = str_to_num(str);
    if (size <= 0) {
        std::fprintf(stderr, "Invalid tlb size: %s\n", str.c_str());
        print_usage(true);
    }

    return size;
}

auto parse_cost(const std::string& str) -> uint32_t {
    uint32_t cost = str_to_num(str);
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

auto parse_addrs(const std::string& addrs) -> std::vector<uint32_t> {
    auto addresses = std::vector<uint32_t>{};

    auto strlist = split_string(addrs, ",");
    for (const auto& str : strlist) {
        if (str.empty()) {
            std::fprintf(stderr, "Invalid address: <empty>. Skipped.\n");
            continue;
        } else {
            uint32_t addr = str_to_num(str);
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

auto addrs_to_string(const std::vector<uint32_t>& addrs) -> std::string {
    if (addrs.empty()) {
        return "<empty>";
    } else {
        std::string result;
        for (const auto& addr : addrs) {
            result += num_to_str_hex(addr) + ",";
        }

        return result.substr(0, result.length() - 1);
    }
}
