// utils.h
// Utilities to handle I/O and display with conversion support.
// Author: Hank Bao

#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

enum class Policy {
    FIFO,
    LRU,
    Random,
};

[[noreturn]] auto print_usage(bool onerror) -> void;

auto split_string(const std::string& s, const std::string& delimiter) -> std::vector<std::string>;

auto begins_with(const std::string& s, const std::string& prefix) -> bool;

auto str_to_num(const std::string& s) -> uint32_t;

auto num_to_str_hex(uint32_t n) -> std::string;

auto policy_to_string(const Policy& policy) -> std::string;

auto parse_page_size(const std::string& str) -> uint32_t;

auto parse_tlb_size(const std::string& str) -> uint32_t;

auto parse_cost(const std::string& str) -> uint32_t;

auto parse_policy(const std::string& policy) -> Policy;

auto parse_addrs(const std::string& addrs) -> std::vector<uint32_t>;

auto addrs_to_string(const std::vector<uint32_t>& addrs) -> std::string;
