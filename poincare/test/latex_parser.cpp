#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/latex_parser/latex_to_layout.h>

#include "helper.h"

using namespace Poincare::Internal;

void assert_latex_layouts_to(const char* latex, const Tree* l) {
  Tree* t = LatexParser::LatexToLayout::Parse(latex);
  quiz_assert_print_if_failure(t->treeIsIdenticalTo(l), latex);
  t->removeTree();
}

QUIZ_CASE(poincare_latex_layout) {
  assert_latex_layouts_to("a-b", "a-b"_l);
  assert_latex_layouts_to("\\left(a-b\\right)+2",
                          KRackL(KParenthesisL("a-b"_l), "+"_cl, "2"_cl));
  assert_latex_layouts_to(
      "1+\\left|3+\\left(a-b\\right)+2\\right|+4",
      KRackL(
          "1"_cl, "+"_cl,
          KAbsL(KRackL("3"_cl, "+"_cl, KParenthesisL("a-b"_l), "+"_cl, "2"_cl)),
          "+"_cl, "4"_cl));
  assert_latex_layouts_to(
      "\\frac{\\sqrt{4}}{\\left(3^{5}\\right)}",
      KRackL(KFracL(KRackL(KSqrtL("4"_l)),
                    KRackL(KParenthesisL("3"_l ^ KSuperscriptL("5"_l))))));
  assert_latex_layouts_to(
      "\\int_{1}^{2}x^{3}\\ dx",
      KRackL(KIntegralL("x"_l, "1"_l, "2"_l, "x"_cl ^ KSuperscriptL("3"_l))));
}
