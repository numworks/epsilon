#include <apps/shared/function_name_helper.h>
#include <apps/shared/global_context.h>
#include <poincare/code_points.h>
#include <poincare/context.h>
#include <poincare/helpers/symbol.h>
#include <quiz.h>

#include <cmath>

#include "helper.h"
#include "shared/continuous_function_store.h"

namespace Graph {

using namespace Shared;
using namespace Poincare::CodePoints;

void assert_default_name_for_symbol_is(CodePoint symbol,
                                       const char* expectedName) {
  constexpr size_t bufferSize = Poincare::SymbolHelper::k_maxNameLength;
  char buffer[bufferSize];
  FunctionNameHelper::DefaultName(buffer, bufferSize, symbol);
  quiz_assert(strcmp(expectedName, buffer) == 0);
}

QUIZ_CASE(graph_list_default_function_name) {
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "f");
  assert_default_name_for_symbol_is(k_polarSymbol, "r1");
  AddFunction("f(x)=x");
  AddFunction("r1(θ)=cos(θ)");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "g");
  assert_default_name_for_symbol_is(k_parametricSymbol, "g");
  assert_default_name_for_symbol_is(k_polarSymbol, "r2");
  AddFunction("g(t)=t");
  AddFunction("r2(θ)=cos(θ)");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "h");
  assert_default_name_for_symbol_is(k_parametricSymbol, "h");
  assert_default_name_for_symbol_is(k_polarSymbol, "r3");
  AddFunction("h(t)=t");
  AddFunction("r3(θ)=cos(θ)");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "p");
  assert_default_name_for_symbol_is(k_parametricSymbol, "p");
  assert_default_name_for_symbol_is(k_polarSymbol, "r4");
  AddFunction("p(t)=t");
  AddFunction("r4(θ)=cos(θ)");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f1");
  assert_default_name_for_symbol_is(k_parametricSymbol, "f1");
  assert_default_name_for_symbol_is(k_polarSymbol, "r5");
  AddFunction("f1(t)=t");
  AddFunction("r5(θ)=cos(θ)");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f2");
  assert_default_name_for_symbol_is(k_parametricSymbol, "f2");
  assert_default_name_for_symbol_is(k_polarSymbol, "r6");

  GlobalContextAccessor::ContinuousFunctionStore().removeAll();

  // Parametric components
  AddFunction("fx(x)=x");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "g");
  AddFunction("gy(x)=x");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "h");
  AddFunction("h(x)=x");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "p");
  AddFunction("pyy(x)=x");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "p");
  AddFunction("py(x)=x");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "f1");
  AddFunction("f1(t)=t");
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "f2");
}

void assert_will_display_parametric_name_error(const char* definition,
                                               ContinuousFunction* f,
                                               bool expectedHasError) {
  Poincare::UserExpression expression = Poincare::UserExpression::Parse(
      definition, Poincare::EmptyContext{}, {.preserveInput = true});
  bool hasError =
      FunctionNameHelper::ParametricComponentsNameError(expression, f);
  quiz_assert(hasError == expectedHasError);
}

QUIZ_CASE(graph_list_valid_function_name) {
  /******     Case A: function was not parametric     ******/

  // Case A.1: name taken by current record
  // Case A.1.1: parametric components name are free
  ContinuousFunction* f = AddFunction("f(x)=x");
  assert_will_display_parametric_name_error("f(x)=x+1", f, false);
  assert_will_display_parametric_name_error("f(t)=t+1", f, false);
  assert_will_display_parametric_name_error("f(t)=(cos(t),sin(t))", f, false);
  assert_will_display_parametric_name_error("f(θ)=θ+1", f, false);
  // Case A.1.2: parametric components name are not free
  AddFunction("fx(x)=x");
  assert_will_display_parametric_name_error("f(x)=x+1", f, false);
  assert_will_display_parametric_name_error("f(t)=t+1", f, false);
  assert_will_display_parametric_name_error("f(t)=(cos(t),sin(t))", f, true);
  assert_will_display_parametric_name_error("f(θ)=θ+1", f, false);

  // Case A.2: name taken by another record
  // Case A.2.1: parametric components name are free
  AddFunction("g(x)=x");
  ContinuousFunction* h = AddFunction("h(x)=x");
  assert_will_display_parametric_name_error("g(x)=x+1", h, false);
  assert_will_display_parametric_name_error("g(t)=t+1", h, false);
  assert_will_display_parametric_name_error("g(t)=(cos(t),sin(t))", h, false);
  assert_will_display_parametric_name_error("g(θ)=θ+1", h, false);
  // Case A.2.2: parametric components name are not free
  AddFunction("gx(x)=x");
  assert_will_display_parametric_name_error("g(x)=x+1", h, false);
  assert_will_display_parametric_name_error("g(t)=t+1", h, false);
  assert_will_display_parametric_name_error("g(t)=(cos(t),sin(t))", h, false);
  assert_will_display_parametric_name_error("g(θ)=θ+1", h, false);

  // Case A.3: name free
  // Case A.3.1: parametric components name are free
  assert_will_display_parametric_name_error("p(x)=x+1", h, false);
  assert_will_display_parametric_name_error("p(t)=t+1", h, false);
  assert_will_display_parametric_name_error("p(t)=(cos(t),sin(t))", h, false);
  assert_will_display_parametric_name_error("p(θ)=θ+1", h, false);
  // Case A.3.2: parametric components name are not free
  AddFunction("py(x)=x");
  assert_will_display_parametric_name_error("p(x)=x+1", h, false);
  assert_will_display_parametric_name_error("p(t)=t+1", h, false);
  assert_will_display_parametric_name_error("p(t)=(cos(t),sin(t))", h, true);
  assert_will_display_parametric_name_error("p(θ)=θ+1", h, false);

  GlobalContextAccessor::ContinuousFunctionStore().removeAll();

  /******       Case B: function was parametric       ******/

  // Case B.1: name taken by current record
  // Case B.1.1: parametric components name are free
  f = AddFunction("f(t)=(cos(t),sin(t))");
  assert_will_display_parametric_name_error("f(x)=x+1", f, false);
  assert_will_display_parametric_name_error("f(t)=t+1", f, false);
  assert_will_display_parametric_name_error("f(t)=(cos(t),sin(t))", f, false);
  assert_will_display_parametric_name_error("f(θ)=θ+1", f, false);
  // Case B.1.2: parametric components name are not free
  AddFunction("fx(x)=x");
  assert_will_display_parametric_name_error("f(x)=x+1", f, false);
  assert_will_display_parametric_name_error("f(t)=t+1", f, false);
  assert_will_display_parametric_name_error("f(t)=(cos(t),sin(t))", f, false);
  assert_will_display_parametric_name_error("f(θ)=θ+1", f, false);

  // Case B.2: name taken by another record
  // Case B.2.1: parametric components name are free
  AddFunction("g(x)=x");
  h = AddFunction("h(t)=(cos(t),sin(t))");
  assert_will_display_parametric_name_error("g(x)=x+1", h, false);
  assert_will_display_parametric_name_error("g(t)=t+1", h, false);
  assert_will_display_parametric_name_error("g(t)=(cos(t),sin(t))", h, false);
  assert_will_display_parametric_name_error("g(θ)=θ+1", h, false);
  // Case B.2.2: parametric components name are not free
  AddFunction("gx(x)=x");
  assert_will_display_parametric_name_error("g(x)=x+1", h, false);
  assert_will_display_parametric_name_error("g(t)=t+1", h, false);
  assert_will_display_parametric_name_error("g(t)=(cos(t),sin(t))", h, false);
  assert_will_display_parametric_name_error("g(θ)=θ+1", h, false);

  // Case B.3: name free
  // Case B.3.1: parametric components name are free
  assert_will_display_parametric_name_error("p(x)=x+1", h, false);
  assert_will_display_parametric_name_error("p(t)=t+1", h, false);
  assert_will_display_parametric_name_error("p(t)=(cos(t),sin(t))", h, false);
  assert_will_display_parametric_name_error("p(θ)=θ+1", h, false);
  // Case B.3.2: parametric components name are not free
  AddFunction("py(x)=x");
  assert_will_display_parametric_name_error("p(x)=x+1", h, false);
  assert_will_display_parametric_name_error("p(t)=t+1", h, false);
  assert_will_display_parametric_name_error("p(t)=(cos(t),sin(t))", h, true);
  assert_will_display_parametric_name_error("p(θ)=θ+1", h, false);
}

}  // namespace Graph
