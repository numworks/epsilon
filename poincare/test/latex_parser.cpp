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
  assert_latex_layouts_to("a\\ -{\\ b}+\\operatorname{re}(\\text{i})",
                          "a-b+re(i)"_l);
  assert_latex_layouts_to("\\left(a-b\\right)+2",
                          KParenthesesL("a-b"_l) ^ "+2"_l);
  assert_latex_layouts_to(
      "1+\\left|3+\\left(a-b\\right)+2\\right|+4",
      "1+"_l ^ KAbsL("3+"_l ^ KParenthesesL("a-b"_l) ^ "+2"_l) ^ "+4"_l);
  assert_latex_layouts_to(
      "\\frac{\\sqrt{4}}{\\left(3^{5}\\right)}",
      KRackL(KFracL(KRackL(KSqrtL("4"_l)),
                    KRackL(KParenthesesL("3"_l ^ KSuperscriptL("5"_l))))));
  assert_latex_layouts_to(
      "\\int_{1}^{2}t^{3}\\ dt",
      KRackL(KIntegralL("t"_l, "1"_l, "2"_l, "t"_l ^ KSuperscriptL("3"_l))));
  assert_latex_layouts_to("\\le\\ge\\cdot\\times\\to\\div\\infty",
                          KCodePointL<UCodePointInferiorEqual>() ^
                              KCodePointL<UCodePointSuperiorEqual>() ^
                              KCodePointL<UCodePointMiddleDot>() ^
                              KCodePointL<UCodePointMultiplicationSign>() ^
                              KCodePointL<UCodePointRightwardsArrow>() ^
                              KCodePointL<'/'>() ^
                              KCodePointL<UCodePointInfinity>());
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
      "1+"_l ^ KAbsL("3+"_l ^ KParenthesesL("a-b"_l) ^ "+2"_l) ^ "+4"_l,
      "1+\\left|3+\\left(a-b\\right)+2\\right|+4");
  assert_layout_convert_to_latex(
      KRackL(KFracL(KRackL(KSqrtL("4"_l)),
                    KRackL(KParenthesesL("3"_l ^ KSuperscriptL("5"_l))))),
      "\\frac{\\sqrt{4}}{\\left(3^{5}\\right)}");
  assert_layout_convert_to_latex(
      KRackL(KIntegralL("t"_l, "1"_l, "2"_l, "t"_l ^ KSuperscriptL("3"_l))),
      "\\int_{1}^{2}t^{3}\\ dt");
  assert_layout_convert_to_latex("12"_l ^ KThousandSeparatorL ^ "345"_l,
                                 "12\\ 345");
  assert_layout_convert_to_latex(
      KRackL(KDiffL("t"_l, "2"_l, "t"_l ^ KSuperscriptL("3"_l))),
      "diff(t^{3},t,2)");
  assert_layout_convert_to_latex(
      KCodePointL<UCodePointInferiorEqual>() ^
          KCodePointL<UCodePointSuperiorEqual>() ^
          KCodePointL<UCodePointMiddleDot>() ^
          KCodePointL<UCodePointMultiplicationSign>() ^
          KCodePointL<UCodePointRightwardsArrow>() ^
          KCodePointL<UCodePointInfinity>(),
      "\\le \\ge \\cdot \\times \\to \\infty ");
}
