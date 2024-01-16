#include <apps/graph/list/function_name_helper.h>
#include <apps/shared/global_context.h>
#include <quiz.h>

#include <cmath>

#include "helper.h"

namespace Graph {

constexpr CodePoint k_cartesianSymbol =
    Shared::ContinuousFunction::k_cartesianSymbol;
constexpr CodePoint k_parametricSymbol =
    Shared::ContinuousFunction::k_parametricSymbol;
constexpr CodePoint k_polarSymbol = Shared::ContinuousFunction::k_polarSymbol;

void assert_default_name_for_symbol_is(CodePoint symbol,
                                       const char* expectedName) {
  constexpr size_t bufferSize = SymbolAbstractNode::k_maxNameLength;
  char buffer[bufferSize];
  FunctionNameHelper::DefaultName(buffer, bufferSize, symbol);
  quiz_assert(strcmp(expectedName, buffer) == 0);
}

QUIZ_CASE(graph_list_function_names) {
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

}  // namespace Graph
