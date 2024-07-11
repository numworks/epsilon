#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/parsing/latex_parser.h>

#include "helper.h"

using namespace Poincare::Internal;

void assert_latex_layouts_to(const char* latex, const Tree* expectedLayout) {
  Tree* l = LatexParser::LatexToLayout(latex);
  quiz_assert_print_if_failure(l->treeIsIdenticalTo(expectedLayout), latex);
  l->removeTree();
}

QUIZ_CASE(pcj_latex_to_layout) {
  assert_latex_layouts_to("a-b", "a-b"_l);
  assert_latex_layouts_to("a\\ -{\\ b}", "a-b"_l);
  assert_latex_layouts_to("\\left(a-b\\right)+2",
                          KRackL(KParenthesesL("a-b"_l), "+"_cl, "2"_cl));
  assert_latex_layouts_to(
      "1+\\left|3+\\left(a-b\\right)+2\\right|+4",
      KRackL(
          "1"_cl, "+"_cl,
          KAbsL(KRackL("3"_cl, "+"_cl, KParenthesesL("a-b"_l), "+"_cl, "2"_cl)),
          "+"_cl, "4"_cl));
  assert_latex_layouts_to(
      "\\frac{\\sqrt{4}}{\\left(3^{5}\\right)}",
      KRackL(KFracL(KRackL(KSqrtL("4"_l)),
                    KRackL(KParenthesesL("3"_l ^ KSuperscriptL("5"_l))))));
  assert_latex_layouts_to(
      "\\int_{1}^{2}t^{3}\\ dt",
      KRackL(KIntegralL("t"_l, "1"_l, "2"_l, "t"_cl ^ KSuperscriptL("3"_l))));
}

void assert_layout_convert_to_latex(const Tree* l, const char* latex) {
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  LatexParser::LayoutToLatex(Rack::From(l), buffer, buffer + bufferSize - 1);
  quiz_assert_print_if_failure(strncmp(buffer, latex, strlen(latex)) == 0,
                               latex);
}

QUIZ_CASE(pcj_layout_to_latex) {
  assert_layout_convert_to_latex(
      KRackL(
          "1"_cl, "+"_cl,
          KAbsL(KRackL("3"_cl, "+"_cl, KParenthesesL("a-b"_l), "+"_cl, "2"_cl)),
          "+"_cl, "4"_cl),
      "1+\\left|3+\\left(a-b\\right)+2\\right|+4");
  assert_layout_convert_to_latex(
      KRackL(KFracL(KRackL(KSqrtL("4"_l)),
                    KRackL(KParenthesesL("3"_l ^ KSuperscriptL("5"_l))))),
      "\\frac{\\sqrt{4}}{\\left(3^{5}\\right)}");
  assert_layout_convert_to_latex(
      KRackL(KIntegralL("t"_l, "1"_l, "2"_l, "t"_cl ^ KSuperscriptL("3"_l))),
      "\\int_{1}^{2}t^{3}\\ dt");
  assert_layout_convert_to_latex("12"_l ^ KThousandSeparatorL ^ "345"_l,
                                 "12\\ 345");
  assert_layout_convert_to_latex(
      KRackL(KDiffL("t"_l, "2"_l, "t"_cl ^ KSuperscriptL("3"_l))),
      "diff(t^{3},t,2)");
}
