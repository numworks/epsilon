#include <emscripten/bind.h>
#include <poincare/old/junior_expression.h>

using namespace emscripten;

namespace Poincare::JSBridge {

EMSCRIPTEN_BINDINGS(junior_expression) {
  class_<PoolHandle>("PCR_PoolHandle")
      .function("isUninitialized", &PoolHandle::isUninitialized);
  class_<OExpression, base<PoolHandle>>("PCR_OExpression");
  class_<JuniorExpression, base<OExpression>>("PCR_Expression");
}

}  // namespace Poincare::JSBridge
