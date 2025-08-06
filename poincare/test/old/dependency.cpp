#include <apps/shared/global_context.h>
#include <ion/storage/file_system.h>
#include <poincare/expression.h>

#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_no_dependency) {
  Shared::GlobalContext context;
  assert_reduce_and_store("1→a", context);

  assert_parsed_expression_simplify_to("1+2", "3", context);
  assert_parsed_expression_simplify_to("a", "1", context);
  assert_parsed_expression_simplify_to("a+1", "2", context);
}

QUIZ_CASE(poincare_dependency_function) {
  Shared::GlobalContext context;
  assert_reduce_and_store("x+1→f(x)", context);
  assert_reduce_and_store("3→g(x)", context);
  assert_reduce_and_store("1+2→a", context);

  // A variable argument is used as dependency only if needed
  assert_parsed_expression_simplify_to("f(x)", "x+1", context);
  assert_parsed_expression_simplify_to("f(x+y)", "x+y+1", context);
  assert_parsed_expression_simplify_to("g(x)", "3", context);
  assert_parsed_expression_simplify_to("g(x+a+3+4)", "3", context);
  // A constant argument does not create dependencies
  assert_parsed_expression_simplify_to("f(2)", "3", context);
  assert_parsed_expression_simplify_to("g(-1.23)", "3", context);
  // Dependencies makes sure f(undef) = undef for all f
  assert_parsed_expression_simplify_to("f(1/0)", "undef", context);
  assert_parsed_expression_simplify_to("g(1/0)", "undef", context);
}

QUIZ_CASE(poincare_dependency_derivative) {
  Shared::GlobalContext context;
  assert_reduce_and_store("x^2→f(x)", context);
  assert_parsed_expression_simplify_to("diff(cos(x),x,0)", "0", context);
  assert_parsed_expression_simplify_to("diff(2x,x,y)", "2", context);
  assert_parsed_expression_simplify_to("diff(f(x),x,a)", "2×a", context);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  assert_reduce_and_store("3→f(x)", context);
  assert_parsed_expression_simplify_to("diff(cos(x)+f(y),x,0)", "0", context);
  assert_parsed_expression_simplify_to("diff(cos(x)+f(y),x,x)", "-sin(x)",
                                       context);
  assert_reduce_and_store("1/0→y", context);
  assert_parsed_expression_simplify_to("diff(cos(x)+f(y),x,0)", "undef",
                                       context);
}

QUIZ_CASE(poincare_dependency_parametered_expression) {
  Shared::GlobalContext context;
  /* Dependencies are not bubbled up out of an integral, but they are still
   * present inside the integral. */
  assert_reduce_and_store("1→f(x)", context);
  assert_parsed_expression_simplify_to("int(f(x)+f(a),x,0,1)", "int(2,x,0,1)",
                                       context);
  assert_reduce_and_store("1/0→a", context);
  assert_parsed_expression_simplify_to("int(f(x)+f(a),x,0,1)", "undef",
                                       context);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();

  /* If the derivation is not handled properly, the symbol x could be reduced
   * to undef. */
  assert_reduce_and_store("x→f(x)", context);
  assert_parsed_expression_simplify_to("int(diff(f(x),x,x),x,0,1)",
                                       "int(1,x,0,1)", context);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  /* When trimming dependencies, we must be able to recognize unreduced
   * expression that are nevertheless undefined. */
  assert_reduce_and_store("1→f(x)", context);
  assert_parsed_expression_simplify_to("f(a)", "1", context);
  // Check that deepIsSymbolic works fine and remove parametered dependencies
  assert_parsed_expression_simplify_to("f(sum(1/n,n,1,5))", "1", context);
}

QUIZ_CASE(poincare_dependency_sequence) {
  const char *emptyString = "";
  Ion::Storage::FileSystem::sharedFileSystem
      ->createRecordWithFullNameAndDataChunks(
          "u.seq", reinterpret_cast<const void **>(&emptyString), 0, 0);
  Shared::GlobalContext context;
  assert_reduce_and_store("3→f(x)", context);
  // Sequence are kept in dependency
  assert_parsed_expression_simplify_to("f(u(n))", "dep(3,{u(n)})", context);
  // Except if the sequence can already be approximated.
  assert_parsed_expression_simplify_to("f(u(2))", "undef", context);
}

QUIZ_CASE(poincare_dependency_power) {
  assert_parsed_expression_simplify_to("1/(1/x)", "dep(x,{nonNull(x)})");
  assert_parsed_expression_simplify_to("x/√(x)", "dep(√(x),{-ln(x)/2})");
  assert_parsed_expression_simplify_to("(1+x)/(1+x)", "dep(1,{(x+1)^0})");
  assert_parsed_expression_simplify_to("x^0", "dep(1,{x^0})");
  assert_parsed_expression_simplify_to("e^(ln(x))", "dep(x,{nonNull(x)})");
}

QUIZ_CASE(poincare_dependency_multiplication) {
  assert_parsed_expression_simplify_to("ln(x)-ln(x)",
                                       "dep(0,{0×ln(x),nonNull(x)})");
  assert_parsed_expression_simplify_to("0*random()", "0");
  assert_parsed_expression_simplify_to("0*randint(1,0)",
                                       "dep(0,{randint(1,0)})");
  // Dependency is properly reduced even when containing symbols
  assert_parsed_expression_simplify_to("0x^arcsin(π)",
                                       "dep(0,{0×x^arcsin(π)})");
  assert_parsed_expression_simplify_to("0x^arcsin(π)",
                                       "dep(nonreal,{0×x^arcsin(π)})", User,
                                       Radian, MetricUnitFormat, Real);
}

QUIZ_CASE(poincare_dependency_trigonometry) {
  assert_parsed_expression_simplify_to("arctan(1/x)",
                                       "dep((π×sign(x)-2×arctan(x))/2,{1/x})");
}
