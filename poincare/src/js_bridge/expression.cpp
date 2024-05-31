#include <emscripten/bind.h>
#include <poincare/js_bridge/expression.h>
#include <poincare/old/junior_expression.h>
#include <poincare/old/junior_layout.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/parsing/latex_parser.h>
#include <poincare/src/layout/rack.h>
#include <poincare/src/memory/tree.h>

#include <string>
using namespace emscripten;

namespace Poincare::JSBridge {

// === Helpers ===

JSTree JSTreeBuilder(JuniorExpression expression) {
  return JSTreeBuilder(expression.tree());
}

JuniorExpression JSTreeToExpression(const JSTree& jsTree) {
  // the Tree is removed by the Builder
  return JuniorExpression::Builder(CloneJSTreeOnStack(jsTree));
}

// === Expression functions ===

// TODO: implement ParseLatex on JuniorExpression directly
JSTree Expression::ParseLatex(std::string latex) {
  Internal::Tree* layout = Internal::LatexParser::LatexToLayout(latex.c_str());
  JuniorExpression expression = JuniorExpression();
  if (layout) {
    expression = JuniorExpression::Parse(layout, nullptr);
    layout->removeTree();
  }
  return JSTreeBuilder(expression);
}

// TODO: implement toLatex on JuniorExpression directly
std::string Expression::ToLatex(const JSTree& jsTree) {
  JuniorExpression e = JSTreeToExpression(jsTree);
  constexpr int k_bufferSize = 1024;  // TODO: make this bigger ? or malloc ?
  char buffer[k_bufferSize];
  Internal::LatexParser::LayoutToLatex(
      Internal::Rack::From(
          e.createLayout(Preferences::PrintFloatMode::Decimal, 7, nullptr)
              .tree()),
      buffer, buffer + k_bufferSize - 1);
  return std::string(buffer, strlen(buffer));
}

JSTree Expression::CloneAndSimplify(const JSTree& jsTree) {
  return JSTreeBuilder(
      JSTreeToExpression(jsTree).cloneAndSimplify(ReductionContext()));
}

// TODO: implement approximateToScalar on JuniorExpression directly
double Expression::ApproximateToScalar(const JSTree& jsTree) {
  return JSTreeToExpression(jsTree).approximateToScalarWithValue<double>(NAN);
}

JSTree Expression::GetSystemFunction(const JSTree& jsTree, std::string var) {
  return JSTreeBuilder(
      JSTreeToExpression(jsTree).getSystemFunction(var.c_str()));
}

double Expression::ApproximateToScalarWithValue(const JSTree& jsTree,
                                                double val) {
  return JSTreeToExpression(jsTree).approximateToScalarWithValue<double>(val);
}

// Binding code
EMSCRIPTEN_BINDINGS(expression) {
  class_<Expression>("Expression")
      .class_function("ParseLatex", &Expression::ParseLatex)
      .class_function("CloneAndSimplify", &Expression::CloneAndSimplify)
      .class_function("ToLatex", &Expression::ToLatex)
      .class_function("ApproximateToScalar", &Expression::ApproximateToScalar)
      .class_function("GetSystemFunction", &Expression::GetSystemFunction)
      .class_function("ApproximateToScalarWithValue",
                      &Expression::ApproximateToScalarWithValue);
}

}  // namespace Poincare::JSBridge
