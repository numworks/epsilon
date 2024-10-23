#include <emscripten/bind.h>
#include <poincare/old/junior_expression.h>

using namespace emscripten;

namespace Poincare::JSBridge {

EMSCRIPTEN_BINDINGS(junior_expression) {
  class_<PoolHandle>("PCR_PoolHandle")
      .function("isUninitialized", &PoolHandle::isUninitialized);

  class_<OExpression, base<PoolHandle>>("PCR_OExpression")
      .function("isIdenticalTo", &OExpression::isIdenticalTo);

  class_<JuniorExpression, base<OExpression>>("PCR_Expression")
      .function("isUndefined", &JuniorExpression::isUndefined)
      .function("isNAry", &JuniorExpression::isNAry)
      .function("isApproximate", &JuniorExpression::isApproximate)
      .function("isPlusOrMinusInfinity",
                &JuniorExpression::isPlusOrMinusInfinity)
      .function("isPercent", &JuniorExpression::isPercent)
      .function("isSequence", &JuniorExpression::isSequence)
      .function("isIntegral", &JuniorExpression::isIntegral)
      .function("isDiff", &JuniorExpression::isDiff)
      .function("isBoolean", &JuniorExpression::isBoolean)
      .function("isList", &JuniorExpression::isList)
      .function("isUserSymbol", &JuniorExpression::isUserSymbol)
      .function("isUserFunction", &JuniorExpression::isUserFunction)
      .function("isStore", &JuniorExpression::isStore)
      .function("isFactor", &JuniorExpression::isFactor)
      .function("isPoint", &JuniorExpression::isPoint)
      .function("isNonReal", &JuniorExpression::isNonReal)
      .function("isOpposite", &JuniorExpression::isOpposite)
      .function("isDiv", &JuniorExpression::isDiv)
      .function("isBasedInteger", &JuniorExpression::isBasedInteger)
      .function("isDep", &JuniorExpression::isDep)
      .function("isComparison", &JuniorExpression::isComparison)
      .function("isEquality", &JuniorExpression::isEquality)
      .function("isRational", &JuniorExpression::isRational)
      .function("isDiscontinuous", &JuniorExpression::isDiscontinuous)
      .function("isConstantNumber", &JuniorExpression::isConstantNumber)
      .function("isPureAngleUnit", &JuniorExpression::isPureAngleUnit)
      .function("allChildrenAreUndefined",
                &JuniorExpression::allChildrenAreUndefined)
}

}  // namespace Poincare::JSBridge
