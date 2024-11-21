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
      "\\frac{\\sqrt{4}}{\\left(3^{\\overline{5}}\\right)}",
      KRackL(KFracL(KRackL(KSqrtL("4"_l)),
                    KRackL(KParenthesesL(
                        "3"_l ^ KSuperscriptL(KRackL(KConjL("5"_l))))))));
  // Integral
  assert_latex_layouts_to(
      "\\int_{0}^{1}t^{3}\\ dt",
      KRackL(KIntegralL("t"_l, "0"_l, "1"_l, "t"_l ^ KSuperscriptL("3"_l))));
  assert_latex_layouts_to(
      "\\int_{0}^{1}z^{3}dz+3",
      KRackL(KIntegralL("z"_l, "0"_l, "1"_l, "z"_l ^ KSuperscriptL("3"_l)),
             "+"_cl, "3"_cl));
  /* The first d is inside "round" so it's not used as separator */
  assert_latex_layouts_to(
      "\\int_{0}^{1}round(abcdx)dabcdx",
      KRackL(KIntegralL("abcdx"_l, "0"_l, "1"_l, "round(abcdx)"_l)));
  /* The first d is inside "undef" so it's not used as separator */
  assert_latex_layouts_to(
      "\\int_{0}^{1}uundefndef",
      KRackL(KIntegralL("ef"_l, "0"_l, "1"_l, "uundefn"_l)));
  /* The first d is not before a variable so it's not used as separator */
  assert_latex_layouts_to("\\int_{0}^{1}d(x)dx",
                          KRackL(KIntegralL("x"_l, "0"_l, "1"_l, "d(x)"_l)));
  // Sum
  assert_latex_layouts_to(
      "\\sum_{k=0}^{9}\\left(t^{3}\\right)",
      KRackL(KSumL("k"_l, "0"_l, "9"_l, "t"_l ^ KSuperscriptL("3"_l))));
  // Product
  assert_latex_layouts_to(
      "\\left(\\prod_{k=0}^{9}t^{3}+1\\right)+1",
      KParenthesesL(KRackL(KProductL("k"_l, "0"_l, "9"_l,
                                     "t"_l ^ KSuperscriptL("3"_l) ^ "+1"_l))) ^
          "+1"_l);

  // Diff
  assert_latex_layouts_to(
      "\\frac{d}{dx}x^{3}",
      KRackL(KDiffL("x"_l, "x"_l, "1"_l, "x"_l ^ KSuperscriptL("3"_l))));
  assert_latex_layouts_to(
      "\\frac{d}{dx}\\left(x^{3}\\right)_{x=2}",
      KRackL(KDiffL("x"_l, "2"_l, "1"_l, "x"_l ^ KSuperscriptL("3"_l))));
  assert_latex_layouts_to(
      "\\frac{d^{3}}{dx^{3}}x^{3}",
      KRackL(KNthDiffL("x"_l, "x"_l, "3"_l, "x"_l ^ KSuperscriptL("3"_l))));
  assert_latex_layouts_to(
      "\\frac{d^{3}}{dx^{3}}\\left(x^{3}\\right)_{x=2}",
      KRackL(KNthDiffL("x"_l, "2"_l, "3"_l, "x"_l ^ KSuperscriptL("3"_l))));

  // Symbols
  assert_latex_layouts_to("\\le\\ge\\cdot\\times\\degree\\to\\div\\infty",
                          KCodePointL<UCodePointInferiorEqual>() ^
                              KCodePointL<UCodePointSuperiorEqual>() ^
                              KCodePointL<UCodePointMiddleDot>() ^
                              KCodePointL<UCodePointMultiplicationSign>() ^
                              KCodePointL<UCodePointDegreeSign>() ^
                              KCodePointL<UCodePointRightwardsArrow>() ^
                              KCodePointL<'/'>() ^
                              KCodePointL<UCodePointInfinity>());
}

void assert_layout_convert_to_latex(const Tree* l, const char* latex,
                                    bool withThousandsSeparator = false) {
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  LatexParser::LayoutToLatex(Rack::From(l), buffer, buffer + bufferSize - 1,
                             withThousandsSeparator);
  quiz_assert_print_if_failure(strncmp(buffer, latex, strlen(latex)) == 0,
                               latex);
}

QUIZ_CASE(pcj_layout_to_latex) {
  assert_layout_convert_to_latex(
      "1+"_l ^ KAbsL("3+"_l ^ KParenthesesL("a-b"_l) ^ "+2"_l) ^ "+4"_l,
      "1+\\left|3+\\left(a-b\\right)+2\\right|+4");
  assert_layout_convert_to_latex(
      KRackL(KFracL(
          KRackL(KSqrtL("4"_l)),
          KRackL(KParenthesesL("3"_l ^ KSuperscriptL(KRackL(KConjL("5"_l))))))),
      "\\frac{\\sqrt{4}}{\\left(3^{\\overline{5}}\\right)}");

  // Integral
  assert_layout_convert_to_latex(
      KRackL(KIntegralL("t"_l, "1"_l, "2"_l, "t"_l ^ KSuperscriptL("3"_l)),
             "+"_cl, "3"_cl),
      "\\int_{1}^{2}t^{3}\\ dt +3");

  // Sum
  assert_layout_convert_to_latex(
      KRackL(KSumL("k"_l, "0"_l, "9"_l, "t"_l ^ KSuperscriptL("3"_l))),
      "\\sum_{k=0}^{9}\\left(t^{3}\\right)");
  // Product
  assert_layout_convert_to_latex(
      KParenthesesL(KRackL(KProductL("k"_l, "0"_l, "9"_l,
                                     "t"_l ^ KSuperscriptL("3"_l) ^ "+1"_l))) ^
          "+1"_l,
      "\\left(\\prod_{k=0}^{9}\\left(t^{3}+1\\right)\\right)+1");

  // Diff
  assert_layout_convert_to_latex(
      KRackL(KDiffL("x"_l, "x"_l, "1"_l, "x"_l ^ KSuperscriptL("3"_l))),
      "\\frac{d}{dx}\\left(x^{3}\\right)");
  assert_layout_convert_to_latex(
      KRackL(KDiffL("x"_l, "2"_l, "1"_l, "x"_l ^ KSuperscriptL("3"_l))),
      "\\frac{d}{dx}\\left(x^{3}\\right)_{x=2}");
  assert_layout_convert_to_latex(
      KRackL(KNthDiffL("x"_l, "x"_l, "3"_l, "x"_l ^ KSuperscriptL("3"_l))),
      "\\frac{d^{3}}{dx^{3}}\\left(x^{3}\\right)");
  assert_layout_convert_to_latex(
      KRackL(KNthDiffL("x"_l, "2"_l, "3"_l, "x"_l ^ KSuperscriptL("3"_l))),
      "\\frac{d^{3}}{dx^{3}}\\left(x^{3}\\right)_{x=2}");

  // Test the thousand separators
  const Tree* layoutWithThousands = "12"_l ^ KThousandsSeparatorL ^ "345"_l;
  assert_layout_convert_to_latex(layoutWithThousands, "12\\ 345", true);
  assert_layout_convert_to_latex(layoutWithThousands, "12345", false);

  assert_layout_convert_to_latex(
      KCodePointL<UCodePointInferiorEqual>() ^
          KCodePointL<UCodePointSuperiorEqual>() ^
          KCodePointL<UCodePointMiddleDot>() ^
          KCodePointL<UCodePointMultiplicationSign>() ^
          KCodePointL<UCodePointDegreeSign>() ^
          KCodePointL<UCodePointRightwardsArrow>() ^
          KCodePointL<UCodePointInfinity>(),
      "\\le \\ge \\cdot \\times \\degree \\to \\infty ");
}
