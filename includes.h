#pragma once

// Remove Windows-specific defines and add Linux ones
#undef __NT__
#undef __X64__

// Linux-specific configuration
#define __LINUX__ 1
#define __EA64__ 1    // Use IDA's official 64-bit define
#define __IDP__ 1
#define __PLUGIN__ 1  // Required for plugin exports

// Enable dangerous API functions (required for full SDK access)
#define USE_DANGEROUS_FUNCTIONS 1

// Plugin configuration
#define plugin_name "IDA RPC"

// IDA headers (keep alphabetical order)
#include <allins.hpp>
#include <auto.hpp>
#include <bytes.hpp>
#include <demangle.hpp>
#include <diskio.hpp>
#include <entry.hpp>
#include <expr.hpp>
#include <frame.hpp>
#include <ida.hpp>
#include <idp.hpp>
#include <kernwin.hpp>
#include <loader.hpp>
#include <name.hpp>
#include <search.hpp>
#include <segment.hpp>
#include <typeinf.hpp>
#include <ua.hpp>

// Add Linux-specific headers
#ifdef __LINUX__
#include <pro.h>       // Contains Linux-specific memory management
#include <link.h>      // Needed for ELF binary handling
#endif