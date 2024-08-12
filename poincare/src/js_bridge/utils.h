#ifndef POINCARE_JS_BRIDGE_UTILS_H
#define POINCARE_JS_BRIDGE_UTILS_H

#include <emscripten/bind.h>
#include <poincare/old/junior_expression.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::JSBridge {

namespace Utils {

Internal::Tree* JsArrayToTree(const emscripten::val& jsTree);
emscripten::val TreeToJsArray(const Internal::Tree* tree);

}  // namespace Utils
}  // namespace Poincare::JSBridge
#endif
