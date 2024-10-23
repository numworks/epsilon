
#include "utils.h"

#include <emscripten/bind.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/src/memory/type_block.h>

using namespace Poincare::Internal;
using namespace emscripten;

namespace Poincare::JSBridge {

namespace Utils {

bool ArraysHaveSameLength(const FloatArray& array1, const FloatArray& array2) {
  return array1["length"].as<int>() == array2["length"].as<int>();
}

EMSCRIPTEN_BINDINGS(float_array) { register_type<FloatArray>("number[]"); }

}  // namespace Utils
}  // namespace Poincare::JSBridge
