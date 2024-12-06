#include <apps/shared/function_name_helper.h>
#include <apps/shared/global_context.h>
#include <poincare/code_points.h>
#include <quiz.h>

#include <cmath>

#include "helper.h"

namespace Graph {

using namespace Shared;

constexpr CodePoint k_cartesianSymbol = Poincare::CodePoints::k_cartesianSymbol;
constexpr CodePoint k_parametricSymbol =
    Poincare::CodePoints::k_parametricSymbol;
constexpr CodePoint k_polarSymbol = Poincare::CodePoints::k_polarSymbol;

void assert_default_name_for_symbol_is(CodePoint symbol,
                                       const char* expectedName) {
  constexpr size_t bufferSize = Poincare::SymbolAbstractNode::k_maxNameLength;
  char buffer[bufferSize];
  FunctionNameHelper::DefaultName(buffer, bufferSize, symbol);
  quiz_assert(strcmp(expectedName, buffer) == 0);
}

QUIZ_CASE(graph_list_default_function_name) {
  GlobalContext context;
  ContinuousFunctionStore store;

  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "f");
  assert_default_name_for_symbol_is(k_polarSymbol, "r1");
  addFunction("f(x)=x", &store, &context);
  addFunction("r1(θ)=cos(θ)", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "g");
  assert_default_name_for_symbol_is(k_parametricSymbol, "g");
  assert_default_name_for_symbol_is(k_polarSymbol, "r2");
  addFunction("g(t)=t", &store, &context);
  addFunction("r2(θ)=cos(θ)", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "h");
  assert_default_name_for_symbol_is(k_parametricSymbol, "h");
  assert_default_name_for_symbol_is(k_polarSymbol, "r3");
  addFunction("h(t)=t", &store, &context);
  addFunction("r3(θ)=cos(θ)", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "p");
  assert_default_name_for_symbol_is(k_parametricSymbol, "p");
  assert_default_name_for_symbol_is(k_polarSymbol, "r4");
  addFunction("p(t)=t", &store, &context);
  addFunction("r4(θ)=cos(θ)", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f1");
  assert_default_name_for_symbol_is(k_parametricSymbol, "f1");
  assert_default_name_for_symbol_is(k_polarSymbol, "r5");
  addFunction("f1(t)=t", &store, &context);
  addFunction("r5(θ)=cos(θ)", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f2");
  assert_default_name_for_symbol_is(k_parametricSymbol, "f2");
  assert_default_name_for_symbol_is(k_polarSymbol, "r6");
  store.removeAll();

  // Parametric components
  addFunction("fx(x)=x", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "g");
  addFunction("gy(x)=x", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "h");
  addFunction("h(x)=x", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "p");
  addFunction("pyy(x)=x", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "p");
  addFunction("py(x)=x", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "f1");
  addFunction("f1(t)=t", &store, &context);
  assert_default_name_for_symbol_is(k_cartesianSymbol, "f");
  assert_default_name_for_symbol_is(k_parametricSymbol, "f2");
  store.removeAll();
}

void assert_will_display_parametric_name_error(const char* definition,
                                               ContinuousFunction* f,
                                               bool expectedHasError) {
  Poincare::Expression expression =
      Poincare::Expression::Parse(definition, nullptr);
  bool hasError =
      FunctionNameHelper::ParametricComponentsNameError(expression, f);
  quiz_assert(hasError == expectedHasError);
}

QUIZ_CASE(graph_list_valid_function_name) {
  GlobalContext context;
  ContinuousFunctionStore store;

  /******     Case A: function was not parametric     ******/

  // Case A.1: name taken by current record
  // Case A.1.1: parametric components name are free
  ContinuousFunction* f = addFunction("f(x)=x", &store, &context);
  assert_will_display_parametric_name_error("f(x)=x+1", f, false);
  assert_will_display_parametric_name_error("f(t)=t+1", f, false);
  assert_will_display_parametric_name_error("f(t)=(cos(t),sin(t))", f, false);
  assert_will_display_parametric_name_error("f(θ)=θ+1", f, false);
  // Case A.1.2: parametric components name are not free
  addFunction("fx(x)=x", &store, &context);
  assert_will_display_parametric_name_error("f(x)=x+1", f, false);
  assert_will_display_parametric_name_error("f(t)=t+1", f, false);
  assert_will_display_parametric_name_error("f(t)=(cos(t),sin(t))", f, true);
  assert_will_display_parametric_name_error("f(θ)=θ+1", f, false);

  // Case A.2: name taken by another record
  // Case A.2.1: parametric components name are free
  addFunction("g(x)=x", &store, &context);
  ContinuousFunction* h = addFunction("h(x)=x", &store, &context);
  assert_will_display_parametric_name_error("g(x)=x+1", h, false);
  assert_will_display_parametric_name_error("g(t)=t+1", h, false);
  assert_will_display_parametric_name_error("g(t)=(cos(t),sin(t))", h, false);
  assert_will_display_parametric_name_error("g(θ)=θ+1", h, false);
  // Case A.2.2: parametric components name are not free
  addFunction("gx(x)=x", &store, &context);
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
  addFunction("py(x)=x", &store, &context);
  assert_will_display_parametric_name_error("p(x)=x+1", h, false);
  assert_will_display_parametric_name_error("p(t)=t+1", h, false);
  assert_will_display_parametric_name_error("p(t)=(cos(t),sin(t))", h, true);
  assert_will_display_parametric_name_error("p(θ)=θ+1", h, false);

  store.removeAll();

  /******       Case B: function was parametric       ******/

  // Case B.1: name taken by current record
  // Case B.1.1: parametric components name are free
  f = addFunction("f(t)=(cos(t),sin(t))", &store, &context);
  assert_will_display_parametric_name_error("f(x)=x+1", f, false);
  assert_will_display_parametric_name_error("f(t)=t+1", f, false);
  assert_will_display_parametric_name_error("f(t)=(cos(t),sin(t))", f, false);
  assert_will_display_parametric_name_error("f(θ)=θ+1", f, false);
  // Case B.1.2: parametric components name are not free
  addFunction("fx(x)=x", &store, &context);
  assert_will_display_parametric_name_error("f(x)=x+1", f, false);
  assert_will_display_parametric_name_error("f(t)=t+1", f, false);
  assert_will_display_parametric_name_error("f(t)=(cos(t),sin(t))", f, false);
  assert_will_display_parametric_name_error("f(θ)=θ+1", f, false);

  // Case B.2: name taken by another record
  // Case B.2.1: parametric components name are free
  addFunction("g(x)=x", &store, &context);
  h = addFunction("h(t)=(cos(t),sin(t))", &store, &context);
  assert_will_display_parametric_name_error("g(x)=x+1", h, false);
  assert_will_display_parametric_name_error("g(t)=t+1", h, false);
  assert_will_display_parametric_name_error("g(t)=(cos(t),sin(t))", h, false);
  assert_will_display_parametric_name_error("g(θ)=θ+1", h, false);
  // Case B.2.2: parametric components name are not free
  addFunction("gx(x)=x", &store, &context);
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
  addFunction("py(x)=x", &store, &context);
  assert_will_display_parametric_name_error("p(x)=x+1", h, false);
  assert_will_display_parametric_name_error("p(t)=t+1", h, false);
  assert_will_display_parametric_name_error("p(t)=(cos(t),sin(t))", h, true);
  assert_will_display_parametric_name_error("p(θ)=θ+1", h, false);

  store.removeAll();
}

}  // namespace Graph
