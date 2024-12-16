#ifndef POINCARE_JS_BRIDGE_UTILS_H
#define POINCARE_JS_BRIDGE_UTILS_H

#include <emscripten/val.h>
#include <poincare/expression.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::JSBridge {

EMSCRIPTEN_DECLARE_VAL_TYPE(FloatArray);

namespace Utils {

bool ArraysHaveSameLength(const FloatArray& array1, const FloatArray& array2);

}  // namespace Utils
}  // namespace Poincare::JSBridge
#endif
