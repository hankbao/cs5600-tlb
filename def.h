// def.h
// common type definitions
// Author: Hank Bao

#pragma once

#include <utility>
#include <unistd.h>

typedef uint32_t addr_type;
typedef uint32_t size_type;
typedef uint32_t time_type;

// <pfn, valid>
typedef std::pair<size_type, bool> TlbEntry;
