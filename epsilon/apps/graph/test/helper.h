#pragma once

#include <apps/shared/continuous_function.h>
#include <poincare/test/old/helper.h>

#include "../app.h"

namespace Graph {

// Add a function to the global function store
Shared::ContinuousFunction* AddFunction(const char* definition);

}  // namespace Graph
