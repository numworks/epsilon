#pragma once

#include <ion/storage/record.h>

namespace Graph {

// Add a function to the global function store
Ion::Storage::Record AddFunction(const char* definition);

}  // namespace Graph
