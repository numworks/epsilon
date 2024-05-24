#ifndef POINCARE_LAYOUT_LATEX_PARSER_LATEX_TOKENS_H
#define POINCARE_LAYOUT_LATEX_PARSER_LATEX_TOKENS_H

#include "../k_tree.h"
#include "../vertical_offset.h"

namespace Poincare::Internal {

namespace LatexParser {

class Tokens {
 public:
  /* These token description arrays alternate
   *  - A delimiter string (ex: "\\left(" or "\\right)")
   *  - A string containing 1 char that matches the index of the child in the
   * layout (ex: "\0" or "\1")
   * */
  constexpr static const char* parenthesisToken[] = {"\\left(", "\0",
                                                     "\\right)"};
  constexpr static const char* curlyBracesToken[] = {"\\left{", "\0",
                                                     "\\right}"};
  constexpr static const char* absToken[] = {"\\left|", "\0", "\\right|"};
  constexpr static const char* sqrtToken[] = {"\\sqrt{", "\0", "}"};
  constexpr static const char* superscriptToken[] = {"^{", "\0", "}"};
  constexpr static const char* subscriptToken[] = {"_{", "\0", "}"};
  constexpr static const char* fracToken[] = {"\\frac{", "\0", "}{", "\1", "}"};
  // The root's power is at index 0 in latex and 1 in layouts
  constexpr static const char* nthRootToken[] = {"\\sqrt[", "\1", "]{", "\0",
                                                 "}"};
  constexpr static const char* binomToken[] = {"\\binom{", "\0", "}{", "\1",
                                               "}"};
  /* There is no easy way to know the end of an integral in Latex.
   * We rely on the fact that the user makes it end with " dx"
   *  Layout: Integral(\Symbol, \LowerBound, \UpperBound, \Integrand)
   *  Latex: int_{\LowerBound}^{\UpperBound}\Integrand\ dx
   * */
  constexpr static const char* integralToken[] = {"\\int_{", "\1", "}^{",  "\2",
                                                  "}",       "\3", "\\ dx"};

  /* Layout to latex:
   * Node with custom handling:
   *   OperatorSeparator -> suppressed in Latex
   *   ThousandSeparator -> replaced with ' '
   * Node unimplemented (that are serialized instead):
   *   Ceil
   *   Floor
   *   VectorNorm
   *   Conj
   *   CombinedCodePoints
   *   CondensedSum
   *   Diff
   *   NthDiff
   *   Product
   *   Sum
   *   ListSequence
   *   Point2D
   *   Matrix
   *   Piecewise
   *   PtBinomial (not handled by serialization ?)
   *   PtPermute (not handled by serialization ?)
   * */

  using LayoutDetector = bool (*)(const Tree*);
  using LayoutConstructor = Tree* (*)();

  struct LatexToken {
    const char* const* description;
    const int descriptionLength;
    const LayoutDetector detector;
    const LayoutConstructor constructor;
  };

  constexpr static LatexToken k_tokens[] = {
      // Parenthesis
      {parenthesisToken, std::size(parenthesisToken),
       [](const Tree* t) -> bool { return t->isParenthesisLayout(); },
       []() -> Tree* { return KParenthesisL(KRackL())->clone(); }},
      // Curly braces
      {curlyBracesToken, std::size(curlyBracesToken),
       [](const Tree* t) -> bool { return t->isCurlyBraceLayout(); },
       []() -> Tree* { return KCurlyBracesL(KRackL())->clone(); }},
      // Absolute value
      {absToken, std::size(absToken),
       [](const Tree* t) -> bool { return t->isAbsLayout(); },
       []() -> Tree* { return KAbsL(KRackL())->clone(); }},
      // Sqrt
      {sqrtToken, std::size(sqrtToken),
       [](const Tree* t) -> bool { return t->isSqrtLayout(); },
       []() -> Tree* { return KSqrtL(KRackL())->clone(); }},
      // Superscript
      {superscriptToken, std::size(superscriptToken),
       [](const Tree* t) -> bool {
         return t->isVerticalOffsetLayout() && VerticalOffset::IsSuperscript(t);
       },
       // Subscript
       []() -> Tree* { return KSuperscriptL(KRackL())->clone(); }},
      {subscriptToken, std::size(subscriptToken),
       [](const Tree* t) -> bool {
         return t->isVerticalOffsetLayout() && VerticalOffset::IsSubscript(t);
       },
       []() -> Tree* { return KSubscriptL(KRackL())->clone(); }},
      // Fraction
      {fracToken, std::size(fracToken),
       [](const Tree* t) -> bool { return t->isFractionLayout(); },
       []() -> Tree* { return KFracL(KRackL(), KRackL())->clone(); }},
      // Root
      {nthRootToken, std::size(nthRootToken),
       [](const Tree* t) -> bool { return t->isRootLayout(); },
       []() -> Tree* { return KRootL(KRackL(), KRackL())->clone(); }},
      // Binomial
      {binomToken, std::size(binomToken),
       [](const Tree* t) -> bool { return t->isBinomialLayout(); },
       []() -> Tree* { return KBinomialL(KRackL(), KRackL())->clone(); }},
      // Integral
      // For now, you can only integrate in x
      {integralToken, std::size(integralToken),
       [](const Tree* t) -> bool { return t->isIntegralLayout(); },
       []() -> Tree* {
         return KIntegralL("x"_l, KRackL(), KRackL(), KRackL())->clone();
       }},
  };
};

}  // namespace LatexParser

}  // namespace Poincare::Internal

#endif
