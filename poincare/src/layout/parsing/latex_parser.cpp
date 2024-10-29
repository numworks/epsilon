#include "latex_parser.h"

#include <omg/utf8_decoder.h>
#include <poincare/src/layout/code_point_layout.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/layout/vertical_offset.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include <array>

namespace Poincare::Internal {

namespace LatexParser {

constexpr static const char* k_latexSpacing = "\\ ";

// ===== Tokens =====

/* These token description arrays alternate
 *  - A delimiter string (ex: "\\left(" or "\\right)")
 *  - A string containing 1 char that matches the index of the child in the
 * layout (ex: "\0" or "\1")
 * */
constexpr static const char* parenthesisToken[] = {"\\left(", "\0", "\\right)"};
constexpr static const char* curlyBracesToken[] = {"\\left\\{", "\0",
                                                   "\\right\\}"};
constexpr static const char* absToken[] = {"\\left|", "\0", "\\right|"};
constexpr static const char* sqrtToken[] = {"\\sqrt{", "\0", "}"};
constexpr static const char* superscriptToken[] = {"^{", "\0", "}"};
constexpr static const char* subscriptToken[] = {"_{", "\0", "}"};
constexpr static const char* fracToken[] = {"\\frac{", "\0", "}{", "\1", "}"};
// The root's power is at index 0 in latex and 1 in layouts
constexpr static const char* nthRootToken[] = {"\\sqrt[", "\1", "]{", "\0",
                                               "}"};
constexpr static const char* binomToken[] = {"\\binom{", "\0", "}{", "\1", "}"};
/* There is no easy way to know the end of an integral in Latex.
 * We rely on the fact that the user makes it end with " d${var}"
 *  Layout: Integral(\Symbol, \LowerBound, \UpperBound, \Integrand)
 *  Latex: int_{\LowerBound}^{\UpperBound}\Integrand\ d\Symbol
 * This fails:
 * - If the integrand contains a "d"
 * - If the integral isn't followed by a space or at the end of the string
 * */
constexpr static const char* integralToken[] = {
    "\\int_{", "\1", "}^{", "\2", "}", "\3", "\\ d", "\0", k_latexSpacing};

// Code points
constexpr static const char* middleDotToken[] = {"\\cdot"};
constexpr static const char* multiplicationSignToken[] = {"\\times"};
constexpr static const char* lesserOrEqualToken[] = {"\\le"};
constexpr static const char* greaterOrEqualToken[] = {"\\ge"};
constexpr static const char* rightwardsArrowToken[] = {"\\to"};
constexpr static const char* infinityToken[] = {"\\infty"};
constexpr static const char* divisionToken[] = {"\\div"};

// Tokens that do nothing
constexpr static const char* textToken[] = {"\\text{"};
constexpr static const char* operatorToken[] = {"\\operatorname{"};
constexpr static const char* spaceToken[] = {" "};
constexpr static const char* escapeToken[] = {"\\"};
constexpr static const char* leftBraceToken[] = {"{"};
constexpr static const char* rightBraceToken[] = {"}"};

using LayoutDetector = bool (*)(const Tree*);
using LayoutConstructor = Tree* (*)();

struct LatexToken {
  const char* const* description;
  const int descriptionLength;
  const LayoutDetector detector;
  const LayoutConstructor constructor;
};

#define ONE_CHILD_TOKEN(LATEX, IS_LAYOUT, KTREE)               \
  {                                                            \
    LATEX, std::size(LATEX),                                   \
        [](const Tree* t) -> bool { return t->IS_LAYOUT(); },  \
        []() -> Tree* { return KTREE(KRackL())->cloneTree(); } \
  }

#define TWO_CHILDREN_TOKEN(LATEX, IS_LAYOUT, KTREE)                      \
  {                                                                      \
    LATEX, std::size(LATEX),                                             \
        [](const Tree* t) -> bool { return t->IS_LAYOUT(); },            \
        []() -> Tree* { return KTREE(KRackL(), KRackL())->cloneTree(); } \
  }

#define CODEPOINT_TOKEN(LATEX, CODEPOINT)                               \
  {                                                                     \
    LATEX, std::size(LATEX),                                            \
        [](const Tree* t) -> bool {                                     \
          return CodePointLayout::IsCodePoint(t, CODEPOINT);            \
        },                                                              \
        []() -> Tree* { return KCodePointL<CODEPOINT>()->cloneTree(); } \
  }

#define DO_NOTHING_TOKEN(LATEX)                                           \
  {                                                                       \
    LATEX, std::size(LATEX), [](const Tree* t) -> bool { return false; }, \
        []() -> Tree* { return nullptr; }                                 \
  }

constexpr static LatexToken k_tokens[] = {
    // Parenthesis
    ONE_CHILD_TOKEN(parenthesisToken, isParenthesesLayout, KParenthesesL),
    // Curly braces
    ONE_CHILD_TOKEN(curlyBracesToken, isCurlyBracesLayout, KCurlyBracesL),
    // Absolute value
    ONE_CHILD_TOKEN(absToken, isAbsLayout, KAbsL),
    // Sqrt
    ONE_CHILD_TOKEN(sqrtToken, isSqrtLayout, KSqrtL),
    // Superscript
    {superscriptToken, std::size(superscriptToken),
     [](const Tree* l) -> bool {
       return l->isVerticalOffsetLayout() && VerticalOffset::IsSuperscript(l);
     },
     []() -> Tree* { return KSuperscriptL(KRackL())->cloneTree(); }},
    // Subscript
    {subscriptToken, std::size(subscriptToken),
     [](const Tree* l) -> bool {
       return l->isVerticalOffsetLayout() && VerticalOffset::IsSubscript(l);
     },
     []() -> Tree* { return KSubscriptL(KRackL())->cloneTree(); }},
    // Fraction
    TWO_CHILDREN_TOKEN(fracToken, isFractionLayout, KFracL),
    // Root
    TWO_CHILDREN_TOKEN(nthRootToken, isRootLayout, KRootL),
    // Binomial
    TWO_CHILDREN_TOKEN(binomToken, isBinomialLayout, KBinomialL),
    // Integral
    {integralToken, std::size(integralToken),
     [](const Tree* l) -> bool { return l->isIntegralLayout(); },
     []() -> Tree* {
       return KIntegralL(KRackL(), KRackL(), KRackL(), KRackL())->cloneTree();
     }},
    /* WARNING: The order matters here, since we want "\left(" to be checked
     * before "\le" */
    // Middle Dot
    CODEPOINT_TOKEN(middleDotToken, UCodePointMiddleDot),
    // Multiplication sign
    CODEPOINT_TOKEN(multiplicationSignToken, UCodePointMultiplicationSign),
    // <=
    CODEPOINT_TOKEN(lesserOrEqualToken, UCodePointInferiorEqual),
    // >=
    CODEPOINT_TOKEN(greaterOrEqualToken, UCodePointSuperiorEqual),
    // ->
    CODEPOINT_TOKEN(rightwardsArrowToken, UCodePointRightwardsArrow),
    // Infinity
    CODEPOINT_TOKEN(infinityToken, UCodePointInfinity),
    // ÷
    {divisionToken, std::size(divisionToken),
     // This codepoint doesn't exist in Poincare
     [](const Tree* t) -> bool { return false; },
     []() -> Tree* { return KCodePointL<'/'>()->cloneTree(); }},
    // Tokens that do nothing
    DO_NOTHING_TOKEN(textToken),
    DO_NOTHING_TOKEN(operatorToken),
    DO_NOTHING_TOKEN(spaceToken),
    DO_NOTHING_TOKEN(escapeToken),
    DO_NOTHING_TOKEN(leftBraceToken),
    DO_NOTHING_TOKEN(rightBraceToken),
};

// ===== Latex to Layout ======

Tree* NextLatexToken(const char** start);

void ParseLatexOnRackUntilIdentifier(Rack* parent, const char** start,
                                     const char* endIdentifier,
                                     bool isEndIdentifierOptional) {
  size_t endLen = strlen(endIdentifier);
  while (**start != 0 &&
         (endLen == 0 || strncmp(*start, endIdentifier, endLen) != 0)) {
    Tree* child = NextLatexToken(start);
    if (child) {
      NAry::AddChild(parent, child);
    }
  }

  if (**start == 0 && endLen > 0) {
    /* We're at the end of the string and endIdentifier couldn't be found */
    if (isEndIdentifierOptional) {
      return;
    }
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }

  *start += endLen;
}

Tree* NextLatexToken(const char** start) {
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
    for (int i = 1; i < token.descriptionLength - 1; i += 2) {
      assert(strlen(token.description[i]) <= 1);
      int childIndexInLayout = token.description[i][0];
      const char* rightDelimiter = token.description[i + 1];

      /* If the last delimiter is a space, it's only to be separated from
       * the next token, so it can be omitted if at the end of the string.
       * This currently only applies to integral. */
      bool optionalRightDelimiter =
          i == token.descriptionLength - 2 &&
          strncmp(rightDelimiter, k_latexSpacing, strlen(k_latexSpacing)) == 0;

      ParseLatexOnRackUntilIdentifier(
          Rack::From(layoutToken->child(childIndexInLayout)), start,
          rightDelimiter, optionalRightDelimiter);
    }

    return layoutToken;
  }

  // Code points
  UTF8Decoder decoder(*start);
  Tree* codepoint = CodePointLayout::Push(decoder.nextCodePoint());
  *start = decoder.stringPosition();
  return codepoint;
}

Tree* LatexToLayout(const char* latexString) {
  ExceptionTry {
    Tree* result = KRackL()->cloneTree();
    ParseLatexOnRackUntilIdentifier(Rack::From(result), &latexString, "", true);
    return result;
  }
  ExceptionCatch(type) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type);
    }
  }
  return nullptr;
}

// ===== Layout to Latex =====

/* Node with custom handling:
 *   OperatorSeparator -> suppressed in Latex
 *   UnitSeparator -> suppressed in Latex
 *   ThousandsSeparator -> replaced with ' ' or suppressed depending on the
 * `withThousandsSeparator` parameter
 *
 * Node unimplemented (that are serialized instead):
 *   Ceil
 *   Floor
 *   VectorNorm
 *   Conj
 *   CombinedCodePoints
 *   CondensedSum
 *   Diff
 *   Product
 *   Sum
 *   ListSequence
 *   Point2D
 *   Matrix
 *   Piecewise
 *   PtBinomial (not handled by serialization ?)
 *   PtPermute (not handled by serialization ?)
 * */

char* LayoutToLatexWithExceptions(const Rack* rack, char* buffer, char* end,
                                  bool withThousandsSeparators) {
  for (const Tree* child : rack->children()) {
    if (child->isOperatorSeparatorLayout() || child->isUnitSeparatorLayout() ||
        (!withThousandsSeparators && child->isThousandsSeparatorLayout())) {
      // Invisible in latex
      continue;
    }

    if (buffer >= end) {
      break;
    }

    /* We don't want to capture withThousandsSeparators in the lambda
     * (generates more code), so we put the parameter manually. */
    RackSerializer serializer = withThousandsSeparators
      ? [](const Rack* rack, char* buffer, char* end) {
          return LayoutToLatexWithExceptions(rack, buffer, end, true);
        }
      : [](const Rack* rack, char* buffer, char* end) {
          return LayoutToLatexWithExceptions(rack, buffer, end, false);
        };

    /* If withThousandsSeparators is false, we already handled the case where
     * child->isThousandsSeparatorLayout() is true. */
    if (child->isThousandsSeparatorLayout()) {
      // Replace with '\ '
      if (buffer + 1 >= end) {
        // Buffer is too short
        TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
      }
      *buffer = '\\';
      buffer += 1;
      *buffer = ' ';
      buffer += 1;
      *buffer = 0;
      continue;
    }

    bool tokenFound = false;
    for (const LatexToken& token : k_tokens) {
      if (!token.detector(child)) {
        continue;
      }

      int i = 0;
      tokenFound = true;
      bool isCodePoint = token.descriptionLength == 1;

      while (true) {
        const char* delimiter = token.description[i];
        size_t delimiterLength = strlen(delimiter);
        if (buffer + delimiterLength + isCodePoint >= end) {
          // Buffer is too short
          TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
        }
        memcpy(buffer, delimiter, delimiterLength);
        buffer += delimiterLength;
        if (i == token.descriptionLength - 1) {
          if (isCodePoint) {
            /* Add a space after latex codepoints, otherwise the string might
             * not be valid in latex.
             * 3\cdotcos -> NO
             * 3\cdot cos -> YES
             **/
            *buffer = ' ';
            buffer += 1;
          }
          *buffer = 0;
          break;
        }
        assert(strlen(token.description[i + 1]) <= 1);
        int indexOfChildInLayout = token.description[i + 1][0];
        buffer = serializer(Rack::From(child->child(indexOfChildInLayout)),
                            buffer, end);
        i += 2;
      }
    }

    if (tokenFound) {
      continue;
    }

    if (child->isCodePointLayout()) {
      buffer = CodePointLayout::CopyName(child, buffer, end - buffer);
      continue;
    }

    // Use common serialization
    buffer = SerializeLayout(Layout::From(child), buffer, end, serializer);
    *buffer = 0;
  }

  if (buffer >= end) {
    // Buffer is too short
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }

  return buffer;
}

char* LayoutToLatex(const Rack* rack, char* buffer, char* end,
                    bool withThousandsSeparators) {
  ExceptionTry {
    char* result =
        LayoutToLatexWithExceptions(rack, buffer, end, withThousandsSeparators);
    return result;
  }
  ExceptionCatch(type) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type);
    }
    *buffer = 0;
  }
  return buffer;
}

}  // namespace LatexParser

}  // namespace Poincare::Internal
