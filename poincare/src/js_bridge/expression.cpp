#include <emscripten/bind.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree.h>

#include <string>

#include "expression_types.h"

using namespace emscripten;
using namespace Poincare::Internal;

namespace Poincare::JSBridge {

// --- Build expressions from Uint8Array ---

/* Bind JavaScript types
 * https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#custom-val-definitions
 * These types are defined in poincare-partial.js
 * */
EMSCRIPTEN_DECLARE_VAL_TYPE(UserExpressionTree);
EMSCRIPTEN_DECLARE_VAL_TYPE(SystemExpressionTree);
EMSCRIPTEN_DECLARE_VAL_TYPE(SystemFunctionTree);

/* Copies Javascript Uint8Array bytes on the tree stack and then build
 * a tree from it */
Tree* Uint8ArrayToPCRTree(const val& jsTree) {
  size_t treeSize = jsTree["length"].as<size_t>();
  if (treeSize == 0) {
    return nullptr;
  }

  // Copies the tree on the stack
  /* The Uint8Array is copied block per block on the stack because I couldn't
   * find an easy way to access the raw address of the external tree buffer.
   * I tried to use `jsTree["buffer"].as<uint8_t*>` but emscripten wouldn't let
   * me access the pointer. */
  TreeStack* stack = TreeStack::SharedTreeStack;
  Block* destination = stack->lastBlock();
  for (int i = 0; i < treeSize; i++) {
    stack->insertBlock(destination + i, Block(jsTree[i].as<uint8_t>()), true);
  }
  return Tree::FromBlocks(destination);
}

// The following functions are used to build expressions from typed Uint8Array

TypedUserExpression BuildUserExpressionFromJsTree(
    const UserExpressionTree& jsTree) {
  Tree* tree = Uint8ArrayToPCRTree(jsTree);
  JuniorExpression result = JuniorExpression::Builder(tree);
  return *reinterpret_cast<TypedUserExpression*>(&result);
}

TypedSystemExpression BuildSystemExpressionFromJsTree(
    const SystemExpressionTree& jsTree) {
  Tree* tree = Uint8ArrayToPCRTree(jsTree);
  JuniorExpression result = JuniorExpression::Builder(tree);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

TypedSystemFunction BuildSystemFunctionFromJsTree(
    const SystemFunctionTree& jsTree) {
  Tree* tree = Uint8ArrayToPCRTree(jsTree);
  JuniorExpression result = JuniorExpression::Builder(tree);
  return *reinterpret_cast<TypedSystemFunction*>(&result);
}

/* The following functions are used to build Uint8Array from typed expressions
 * They rely on the fact that UserExpressionTree, SystemExpressionTree and
 * SystemFunctionTree classes are globally defined in Javascript. These
 * definitions are done in poincare-partial.js
 * */

val treeToTypedArray(const Tree* tree) {
  if (!tree) {
    uint8_t emptyArray[0];
    return val(typed_memory_view(0, emptyArray));
  }
  return val(typed_memory_view(tree->treeSize(),
                               reinterpret_cast<const uint8_t*>(tree)));
}

UserExpressionTree UserExpressionToJsTree(
    const TypedUserExpression& expression) {
  /* Equivalent to the js code "new UserExpressionTree(typedArray)"
   * This array will be instantiated on javascript heap, allowing it to be
   * properly handled by the js garbage collector */
  return UserExpressionTree(val::global("UserExpressionTree")
                                .new_(treeToTypedArray(expression.tree())));
}

SystemExpressionTree SystemExpressionToJsTree(
    const TypedSystemExpression& expression) {
  return SystemExpressionTree(val::global("SystemExpressionTree")
                                  .new_(treeToTypedArray(expression.tree())));
}

SystemFunctionTree SystemFunctionToJsTree(
    const TypedSystemFunction& expression) {
  return SystemFunctionTree(val::global("SystemFunctionTree")
                                .new_(treeToTypedArray(expression.tree())));
}

// --- Typed expression methods ---

bool ExactAndApproximateExpressionsAreStrictlyEqualWrapper(
    const TypedUserExpression& exact, const TypedUserExpression& approximate,
    Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  ProjectionContext ctx{.m_complexFormat = complexFormat,
                        .m_angleUnit = angleUnit};
  return ExactAndApproximateExpressionsAreStrictlyEqual(exact, approximate,
                                                        &ctx);
}

std::string typedToLatex(const TypedUserExpression& expression,
                         int numberOfSignificantDigits) {
  constexpr int k_bufferSize = 1024;  // TODO: make this bigger ? or malloc ?
  char buffer[k_bufferSize];
  EmptyContext context;
  expression.toLatex(buffer, k_bufferSize, Preferences::PrintFloatMode::Decimal,
                     numberOfSignificantDigits, &context);
  return std::string(buffer, strlen(buffer));
}

std::string typedToLatexWith7Digits(const TypedUserExpression& expression) {
  return typedToLatex(expression, 7);
}

TypedUserExpression typedCloneAndBeautify(
    const TypedSystemExpression& expr,
    const ReductionContext& reductionContext) {
  JuniorExpression result = expr.cloneAndBeautify(reductionContext);
  return *reinterpret_cast<TypedUserExpression*>(&result);
}

TypedSystemFunction typedGetSystemFunction(
    const TypedSystemExpression& expression, std::string symbolName) {
  JuniorExpression result =
      expression.getSystemFunction(symbolName.c_str(), true);
  return *reinterpret_cast<TypedSystemFunction*>(&result);
}

TypedSystemExpression typedCloneAndReduce(
    const TypedUserExpression& expr, const ReductionContext& reductionContext) {
  JuniorExpression result = expr.cloneAndReduce(reductionContext);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

TypedSystemExpression typedGetReducedDerivative(
    const TypedSystemExpression& expression, std::string symbolName,
    int derivationOrder) {
  JuniorExpression result =
      expression.getReducedDerivative(symbolName.c_str(), derivationOrder);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

TypedSystemExpression typedApproximateToTree(
    const TypedSystemExpression& expression,
    const ApproximationContext& approximationContext) {
  JuniorExpression result =
      expression.approximateToTree<double>(approximationContext);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

double typedApproximateToScalarWithValue(const TypedSystemFunction& expr,
                                         double value) {
  return expr.approximateToScalarWithValue(value);
}

double typedApproximateIntegralToScalar(
    const TypedSystemFunction& expr, const TypedSystemExpression& upperBound,
    const TypedSystemExpression& lowerBound) {
  return expr.approximateIntegralToScalar<double>(upperBound, lowerBound);
}

EMSCRIPTEN_BINDINGS(junior_expression) {
  register_type<UserExpressionTree>("UserExpressionTree");
  register_type<SystemExpressionTree>("SystemExpressionTree");
  register_type<SystemFunctionTree>("SystemFunctionTree");

  class_<PoolHandle>("PCR_PoolHandle")
      .function("isUninitialized", &PoolHandle::isUninitialized);
  class_<OExpression, base<PoolHandle>>("PCR_OExpression");
  class_<JuniorExpression, base<OExpression>>("PCR_Expression");

  class_<TypedUserExpression, base<JuniorExpression>>("PCR_UserExpression")
      .constructor<>()
      .class_function("BuildFromTree", &BuildUserExpressionFromJsTree)
      .class_function("ExactAndApproximateExpressionsAreStrictlyEqual",
                      &ExactAndApproximateExpressionsAreStrictlyEqualWrapper)
      .function("getTree", &UserExpressionToJsTree)
      .function("toLatex", &typedToLatex)
      .function("toLatex", &typedToLatexWith7Digits)
      .function("cloneAndReduce", &typedCloneAndReduce);

  class_<TypedSystemExpression, base<JuniorExpression>>("PCR_SystemExpression")
      .constructor<>()
      .class_function("BuildFromTree", &BuildSystemExpressionFromJsTree)
      .function("getTree", &SystemExpressionToJsTree)
      .function("cloneAndBeautify", &typedCloneAndBeautify)
      .function("getSystemFunction", &typedGetSystemFunction)
      .function("getReducedDerivative", &typedGetReducedDerivative)
      .function("approximateToTree", &typedApproximateToTree);

  class_<TypedSystemFunction, base<JuniorExpression>>("PCR_SystemFunction")
      .constructor<>()
      .class_function("BuildFromTree", &BuildSystemFunctionFromJsTree)
      .function("getTree", &SystemFunctionToJsTree)
      .function("approximateToScalarWithValue",
                &typedApproximateToScalarWithValue)
      .function("approximateIntegralToScalar",
                &typedApproximateIntegralToScalar);
}

}  // namespace Poincare::JSBridge
