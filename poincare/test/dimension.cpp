#include <apps/shared/global_context.h>
#include <poincare/src/expression/dimension.h>

#include "helper.h"

using namespace Poincare::Internal;

bool dim(const Tree* e, Dimension d, Poincare::Context* ctx = nullptr) {
  return Dimension::DeepCheck(e, ctx) && d == Dimension::Get(e, ctx);
}

bool dim(const char* input, Dimension d, Poincare::Context* ctx = nullptr) {
  Tree* e = TextToTree(input);
  bool result = dim(e, d, ctx);
  e->removeTree();
  return result;
}

bool len(const Tree* e, int n, Poincare::Context* ctx = nullptr) {
  assert(Dimension::DeepCheck(e, ctx));
  return Dimension::ListLength(e, ctx) == n;
}

bool len(const char* input, int n, Poincare::Context* ctx = nullptr) {
  Tree* e = TextToTree(input);
  bool result = len(e, n, ctx);
  e->removeTree();
  return result;
}

bool hasInvalidDimOrLen(const Tree* e, Poincare::Context* ctx = nullptr) {
  return !Dimension::DeepCheck(e, ctx);
}

bool hasInvalidDimOrLen(const char* input, Poincare::Context* ctx = nullptr) {
  Tree* e = TextToTree(input);
  bool result = hasInvalidDimOrLen(e, ctx);
  e->removeTree();
  return result;
}

QUIZ_CASE(pcj_dimension) {
  auto Scalar = Dimension::Scalar();
  auto Matrix = Dimension::Matrix;
  auto Boolean = Dimension::Boolean();
  auto Point = Dimension::Point();
  QUIZ_ASSERT(dim("piecewise([[2]],True,[[3]])", Matrix(1, 1)));

  QUIZ_ASSERT(hasInvalidDimOrLen("[[1][[[2]]]]"));
  QUIZ_ASSERT(hasInvalidDimOrLen("[[1,2][3,4]]+[[2]]"));
  QUIZ_ASSERT(hasInvalidDimOrLen("cos([[2]])"));
  QUIZ_ASSERT(hasInvalidDimOrLen("1/[[1][3]]"));
  QUIZ_ASSERT(hasInvalidDimOrLen("product([[k,2]], k, 1, n)"));
  QUIZ_ASSERT(hasInvalidDimOrLen("(True, False)"));
  QUIZ_ASSERT(hasInvalidDimOrLen("{2,(1,3)}"));
  QUIZ_ASSERT(hasInvalidDimOrLen("randintnorep(1,10,-1)"));

  QUIZ_ASSERT(dim("1", Scalar));
  QUIZ_ASSERT(dim("cos(sin(1+3))*2^3", Scalar));
  QUIZ_ASSERT(dim("[[1][3]]", Matrix(2, 1)));
  QUIZ_ASSERT(dim("[[1][3]]/3", Matrix(2, 1)));
  QUIZ_ASSERT(dim("ref([[1,2][3,4]])", Matrix(2, 2)));
  QUIZ_ASSERT(dim("inverse(identity(2))", Matrix(2, 2)));
  QUIZ_ASSERT(dim("cross([[1,2,3]],[[1,2,3]])", Matrix(1, 3)));
  QUIZ_ASSERT(dim("transpose([[1,2]])*[[1,2,3]]", Matrix(2, 3)));
  QUIZ_ASSERT(dim("dep([[k,2]], {})", Matrix(1, 2)));
  QUIZ_ASSERT(dim("sum([[k,2]], k, 1, n)", Matrix(1, 2)));
  QUIZ_ASSERT(dim("{}", Scalar));
  QUIZ_ASSERT(dim("sequence(k,k,3)", Scalar));

  QUIZ_ASSERT(dim("{False, False}", Boolean));
  QUIZ_ASSERT(hasInvalidDimOrLen("1 + {False, False}"));
  QUIZ_ASSERT(hasInvalidDimOrLen("1 and {False, False}"));
  QUIZ_ASSERT(dim("True and {False, False}", Boolean));
  QUIZ_ASSERT(dim("True and False", Boolean));
  QUIZ_ASSERT(dim("True or (False xor True)", Boolean));
  QUIZ_ASSERT(hasInvalidDimOrLen("0 and False"));
  QUIZ_ASSERT(dim("0 < 3 and False", Boolean));
  QUIZ_ASSERT(hasInvalidDimOrLen("sort({True, False, True})"));
  QUIZ_ASSERT(hasInvalidDimOrLen("min({True, False, True})"));
  QUIZ_ASSERT(dim("{True, False, True} or {True, True, False}", Boolean));

  QUIZ_ASSERT(len("1", Dimension::k_nonListListLength));
  QUIZ_ASSERT(len("{1,2}", 2));
  QUIZ_ASSERT(len("2*cos({1,2})+3", 2));
  QUIZ_ASSERT(len("sequence(2*k+1,k,4)", 4));
  QUIZ_ASSERT(len("2+mean({1,2})", Dimension::k_nonListListLength));
  QUIZ_ASSERT(len("sort({1,2})", 2));
  QUIZ_ASSERT(len("{}", 0));
  QUIZ_ASSERT(len("{True, False}", 2));
  QUIZ_ASSERT(len("{1,2,3,4}(1,3)", 3));
  QUIZ_ASSERT(len("{1,2,3,4}(0,5)", 4));
  QUIZ_ASSERT(len("{1,2,3,4}(0,0)", 0));
  QUIZ_ASSERT(len("{1,2,3,4}(6,4)", 0));
  QUIZ_ASSERT(hasInvalidDimOrLen("{1,2,3,4}(-2,5)"));
  QUIZ_ASSERT(hasInvalidDimOrLen("{1,2,3,4}(-2,-1)"));

  QUIZ_ASSERT(dim("(2,3)", Point));
  QUIZ_ASSERT(dim("{(2,3)}", Point));
  QUIZ_ASSERT(dim("(2,{1,3})", Point));
  QUIZ_ASSERT(dim("piecewise((1,5),x<1,(x,y))", Point));
  QUIZ_ASSERT(dim("diff((x,2x), x, y)", Point));
  QUIZ_ASSERT(dim("dep((1,2), {(1,3),(3,3)})", Point));
  QUIZ_ASSERT(dim("sequence((k,2),k,3)", Point));
  QUIZ_ASSERT(dim("{(1,3), (2,4)}", Point));
  QUIZ_ASSERT(dim("sort({(1,3), (2,4)})", Point));
  QUIZ_ASSERT(dim("{(1,3), (2,4)}(1)", Point));
  QUIZ_ASSERT(dim("{(1,3), (2,4)}(0,1)", Point));
  QUIZ_ASSERT(dim("{(1,3), (2,4)}(1,0)", Scalar));
  QUIZ_ASSERT(dim("dim({(1,3), (2,4)})", Scalar));
  QUIZ_ASSERT(dim("sort(diff({(x,2x),(1,2)}, x, y))", Point));

  QUIZ_ASSERT(hasInvalidDimOrLen("dep((1,2), {(1,3),3})"));
  QUIZ_ASSERT(hasInvalidDimOrLen("(1,2)+(1,3)"));
  QUIZ_ASSERT(hasInvalidDimOrLen("cos((1,2))"));
  QUIZ_ASSERT(hasInvalidDimOrLen("{(1,3), (2,4)}((2,4))"));

  QUIZ_ASSERT(hasInvalidDimOrLen("{_m}"));
  QUIZ_ASSERT(hasInvalidDimOrLen("{[[1]]}"));
  QUIZ_ASSERT(hasInvalidDimOrLen("{2}*[[1]]"));
  QUIZ_ASSERT(hasInvalidDimOrLen("{2}*_m"));

  // Pow
  QUIZ_ASSERT(dim("[[1,2][3,4]]^3", Matrix(2, 2)));
  QUIZ_ASSERT(hasInvalidDimOrLen("[[1,3]]^3"));
  QUIZ_ASSERT(hasInvalidDimOrLen("[[1][3]]^3"));
  QUIZ_ASSERT(hasInvalidDimOrLen("[[1][3]]^[[4]]"));
  QUIZ_ASSERT(hasInvalidDimOrLen("[[1][3]]^[[4,5]]"));
  QUIZ_ASSERT(hasInvalidDimOrLen("3^[[4,5]]"));

  // Division
  QUIZ_ASSERT(dim("[[1,2]]/3", Matrix(1, 2)));
  QUIZ_ASSERT(hasInvalidDimOrLen("3/[[1,2]]"));
  QUIZ_ASSERT(hasInvalidDimOrLen("[[1][2]]/[[3][4]]"));

  // Sub
  QUIZ_ASSERT(dim("[[1,2]]-[[3,4]]", Matrix(1, 2)));
  QUIZ_ASSERT(hasInvalidDimOrLen("[[1,2]]-[[3][4]]"));
  QUIZ_ASSERT(hasInvalidDimOrLen("[[1,2]]-3"));
  QUIZ_ASSERT(hasInvalidDimOrLen("1-[[3][4]]"));

  Shared::GlobalContext globalContext;
  assert(
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecords() ==
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecordsWithExtension(
          "sys"));
  store("2→a", &globalContext);
  store("{4,2}→b", &globalContext);
  store("(1,3)→c", &globalContext);
  store("33_m→d", &globalContext);
  store("{x,2*x}→g(x)", &globalContext);
  store("(x,2*x)→h(x)", &globalContext);
  store("x+33_m→j(x)", &globalContext);

  QUIZ_ASSERT(dim("a", Scalar, &globalContext));
  QUIZ_ASSERT(dim("c", Point, &globalContext));
  QUIZ_ASSERT(len("h(b)", 2, &globalContext));
  QUIZ_ASSERT(dim("g(a)+b+{1,6}", Scalar, &globalContext));
  QUIZ_ASSERT(hasInvalidDimOrLen("j(d)", &globalContext));
  QUIZ_ASSERT(hasInvalidDimOrLen("j(c)", &globalContext));

  QUIZ_ASSERT(SharedTreeStack->numberOfTrees() == 0);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
}
