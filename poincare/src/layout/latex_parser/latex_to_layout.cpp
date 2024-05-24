#include "latex_to_layout.h"

#include <omg/utf8_decoder.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "../code_point_layout.h"
#include "../k_tree.h"
#include "../rack_layout.h"

namespace Poincare::Internal {

namespace LatexParser {

Tree* LatexToLayout::Parse(const char* latexString) {
  Tree* result = KRackL()->clone();
  ParseOnRackUntilIdentifier(Rack::From(result), &latexString, "");
  return result;
}

void LatexToLayout::ParseOnRackUntilIdentifier(Rack* parent, const char** start,
                                               const char* endIdentifier) {
  size_t endLen = strlen(endIdentifier);
  while (**start != 0 &&
         (endLen == 0 || strncmp(*start, endIdentifier, endLen) != 0)) {
    Tree* child = NextToken(start);
    NAry::AddChild(parent, child);
  }

  if (**start == 0 && endLen > 0) {
    // endIdentifier couldn't be found
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
}

Tree* LatexToLayout::NextToken(const char** start) {
  /* These token description arrays alternate
   *  - A delimiter string (ex: "\\left(" or "\\right)")
   *  - A string containing 1 char that matches the index of the child in the
   * layout (ex: "\0" or "\1")
   * */
  constexpr static const char* parenthesisToken[] = {"\\left(", "\0",
                                                     "\\right)"};
  constexpr static const char* absToken[] = {"\\left|", "\0", "\\right|"};
  constexpr static const char* sqrtToken[] = {"\\sqrt{", "\0", "}"};
  constexpr static const char* superscriptToken[] = {"^{", "\0", "}"};
  constexpr static const char* subscriptToken[] = {"_{", "\0", "}"};
  constexpr static const char* fracToken[] = {"\\frac{", "\0", "}{", "\1", "}"};
  // The root's power is at index 0 in latex and 1 in layouts
  constexpr static const char* nthRootToken[] = {"\\sqrt[", "\1", "]{", "\0",
                                                 "}"};
  /* There is no easy way to know the end of an integral in Latex.
   * We rely on the fact that the user makes it end with " dx"
   *  Layout: Integral(\Symbol, \LowerBound, \UpperBound, \Integrand)
   *  Latex: int_{\LowerBound}^{\UpperBound}\Integrand\ dx
   * */
  constexpr static const char* integralToken[] = {"\\int_{", "\1", "}^{",  "\2",
                                                  "}",       "\3", "\\ dx"};

  using LayoutConstructor = Tree* (*)();
  struct LatexToken {
    const char* const* description;
    const int descriptionLength;
    const LayoutConstructor constructor;
  };

  constexpr LatexToken k_tokens[] = {
      {parenthesisToken, std::size(parenthesisToken),
       []() -> Tree* { return KParenthesisL(KRackL())->clone(); }},
      {absToken, std::size(absToken),
       []() -> Tree* { return KAbsL(KRackL())->clone(); }},
      {sqrtToken, std::size(sqrtToken),
       []() -> Tree* { return KSqrtL(KRackL())->clone(); }},
      {superscriptToken, std::size(superscriptToken),
       []() -> Tree* { return KSuperscriptL(KRackL())->clone(); }},
      {subscriptToken, std::size(subscriptToken),
       []() -> Tree* { return KSubscriptL(KRackL())->clone(); }},
      {fracToken, std::size(fracToken),
       []() -> Tree* { return KFracL(KRackL(), KRackL())->clone(); }},
      {nthRootToken, std::size(nthRootToken),
       []() -> Tree* { return KRootL(KRackL(), KRackL())->clone(); }},
      // For now, you can only integrate in x
      {integralToken, std::size(integralToken),
       []() -> Tree* {
         return KIntegralL("x"_l, KRackL(), KRackL(), KRackL())->clone();
       }},
  };

  for (const LatexToken& token : k_tokens) {
    const char* leftDelimiter = token.description[0];
    size_t leftDelimiterLength = strlen(leftDelimiter);
    if (strncmp(*start, leftDelimiter, leftDelimiterLength) != 0) {
      continue;
    }
    // Special token found
    *start += leftDelimiterLength;
    Tree* layoutToken = token.constructor();

    // Parse children
    for (int i = 1; i < token.descriptionLength; i += 2) {
      assert(strlen(token.description[i]) <= 1);
      int childIndexInLayout = token.description[i][0];
      const char* rightDelimiter = token.description[i + 1];
      ParseOnRackUntilIdentifier(
          Rack::From(layoutToken->child(childIndexInLayout)), start,
          rightDelimiter);
      *start += strlen(rightDelimiter);
    }

    return layoutToken;
  }

  // Code points
  UTF8Decoder decoder(*start);
  Tree* codepoint = CodePointLayout::Push(decoder.nextCodePoint());
  *start = decoder.stringPosition();
  return codepoint;
}

}  // namespace LatexParser

}  // namespace Poincare::Internal
