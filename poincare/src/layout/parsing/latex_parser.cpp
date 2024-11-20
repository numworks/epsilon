#include "latex_parser.h"

#include <omg/utf8_decoder.h>
#include <poincare/old/empty_context.h>
#include <poincare/src/layout/code_point_layout.h>
#include <poincare/src/layout/indices.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/parsing/tokenizer.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/layout/vertical_offset.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include <array>

namespace Poincare::Internal {

namespace LatexParser {

// ===== Latex Tokens =====

/* A LatexToken is an array of LatexTokenChildren
 * A LatexTokenChild is a pair of a left delimiter and the index of the child
 * in the layout.
 * Example:
 * For n-th Root, the latex "\\sqrt[2]{3}"" matches the layout Root(3,2)
 * Thus, the LatexToken is:
 * {{.leftDelimiter = "\\sqrt[", .indexInLayout = 1},
 *  {.leftDelimiter = "]{", .indexInLayout = 0},
 *  {.leftDelimiter = "}", .indexInLayout = k_noChild}};
 * */

struct LatexTokenChild {
  const char* leftDelimiter;
  int indexInLayout;
};

const static int k_noChild = -1;

using LatexToken = const LatexTokenChild*;

constexpr static LatexTokenChild parenthesisToken[] = {
    {.leftDelimiter = "\\left(", .indexInLayout = 0},
    {.leftDelimiter = "\\right)", .indexInLayout = k_noChild}};

constexpr static LatexTokenChild curlyBracesToken[] = {
    {.leftDelimiter = "\\left\\{", .indexInLayout = 0},
    {.leftDelimiter = "\\right\\}", .indexInLayout = k_noChild}};

constexpr static LatexTokenChild absToken[] = {
    {.leftDelimiter = "\\left|", .indexInLayout = 0},
    {.leftDelimiter = "\\right|", .indexInLayout = k_noChild}};

constexpr static LatexTokenChild sqrtToken[] = {
    {.leftDelimiter = "\\sqrt{", .indexInLayout = 0},
    {.leftDelimiter = "}", .indexInLayout = k_noChild}};

constexpr static LatexTokenChild conjugateToken[] = {
    {.leftDelimiter = "\\overline{", .indexInLayout = 0},
    {.leftDelimiter = "}", .indexInLayout = k_noChild}};

constexpr static LatexTokenChild superscriptToken[] = {
    {.leftDelimiter = "^{", .indexInLayout = 0},
    {.leftDelimiter = "}", .indexInLayout = k_noChild}};

constexpr static LatexTokenChild subscriptToken[] = {
    {.leftDelimiter = "_{", .indexInLayout = 0},
    {.leftDelimiter = "}", .indexInLayout = k_noChild}};

constexpr static LatexTokenChild fracToken[] = {
    {.leftDelimiter = "\\frac{", .indexInLayout = Fraction::k_numeratorIndex},
    {.leftDelimiter = "}{", .indexInLayout = Fraction::k_denominatorIndex},
    {.leftDelimiter = "}", .indexInLayout = k_noChild}};

constexpr static LatexTokenChild nthRootToken[] = {
    {.leftDelimiter = "\\sqrt[", .indexInLayout = NthRoot::k_indexIndex},
    {.leftDelimiter = "]{", .indexInLayout = NthRoot::k_radicandIndex},
    {.leftDelimiter = "}", .indexInLayout = k_noChild}};

constexpr static LatexTokenChild binomToken[] = {
    {.leftDelimiter = "\\binom{", .indexInLayout = Binomial::k_nIndex},
    {.leftDelimiter = "}{", .indexInLayout = Binomial::k_kIndex},
    {.leftDelimiter = "}", .indexInLayout = k_noChild}};

/* Latex: \\int_{\LowerBound}^{\UpperBound}\Integrand d\Symbol
 * Layout: Integral(\Symbol, \LowerBound, \UpperBound, \Integrand) */
constexpr static LatexTokenChild integralToken[] = {
    {.leftDelimiter = "\\int_{", .indexInLayout = Integral::k_lowerBoundIndex},
    {.leftDelimiter = "}^{", .indexInLayout = Integral::k_upperBoundIndex},
    {.leftDelimiter = "}", .indexInLayout = Integral::k_integrandIndex},
    {.leftDelimiter = "\\ d", .indexInLayout = Integral::k_differentialIndex},
    {.leftDelimiter = " ", .indexInLayout = k_noChild}};

/* Latex: \\sum_{\Variable=\LowerBound}^{\UpperBound}(\Function)
 * Layout: Sum(\Variable, \LowerBound, \UpperBound, \Function) */
constexpr static LatexTokenChild sumToken[] = {
    {.leftDelimiter = "\\sum_{", .indexInLayout = Parametric::k_variableIndex},
    {.leftDelimiter = "=", .indexInLayout = Parametric::k_lowerBoundIndex},
    {.leftDelimiter = "}^{", .indexInLayout = Parametric::k_upperBoundIndex},
    // Split '}' and the parentheses because the latter are optional
    {.leftDelimiter = "}", .indexInLayout = k_noChild},
    {.leftDelimiter = "\\left(", .indexInLayout = Parametric::k_argumentIndex},
    {.leftDelimiter = "\\right)", .indexInLayout = k_noChild}};

/* Latex: \\prod{\Variable=\LowerBound}^{\UpperBound}(\Function)
 * Layout: Product(\Variable, \LowerBound, \UpperBound, \Function) */
constexpr static LatexTokenChild prodToken[] = {
    {.leftDelimiter = "\\prod_{", .indexInLayout = Parametric::k_variableIndex},
    {.leftDelimiter = "=", .indexInLayout = Parametric::k_lowerBoundIndex},
    {.leftDelimiter = "}^{", .indexInLayout = Parametric::k_upperBoundIndex},
    // Split '}' and the parentheses because the latter are optional
    {.leftDelimiter = "}", .indexInLayout = k_noChild},
    {.leftDelimiter = "\\left(", .indexInLayout = Parametric::k_argumentIndex},
    {.leftDelimiter = "\\right)", .indexInLayout = k_noChild}};

/* Latex: \\frac{d}{d\Variable}(\Function)
 * Layout: Diff(\Variable, \Variable, "1"_l, \Function) */
constexpr static LatexTokenChild diffToken[] = {
    {.leftDelimiter = "\\frac{d}{d",
     .indexInLayout = Derivative::k_variableIndex},
    // Split '}' and the parentheses because the latter are optional
    {.leftDelimiter = "}", .indexInLayout = k_noChild},
    {.leftDelimiter = "\\left(", .indexInLayout = Derivative::k_derivandIndex},
    {.leftDelimiter = "\\right)", .indexInLayout = k_noChild}};

/* Latex: \\frac{d}{d\Variable}(\Function)_{\Variable=\Abscissa}
 * Layout: Diff(\Variable, \Abscissa, "1"_l, \Function) */
constexpr static LatexTokenChild diffAtAToken[] = {
    {.leftDelimiter = "\\frac{d}{d",
     .indexInLayout = Derivative::k_variableIndex},
    {.leftDelimiter = "}\\left(", .indexInLayout = Derivative::k_derivandIndex},
    {.leftDelimiter = "\\right)_{",
     .indexInLayout = Derivative::k_variableIndex},
    {.leftDelimiter = "=", .indexInLayout = Derivative::k_abscissaIndex},
    {.leftDelimiter = "}", .indexInLayout = k_noChild}};

/* Latex: \\frac{d^{\Order}}{d\Variable^{\Order}}(\Function)
 * Layout: Diff(\Variable, \Variable, \Order, \Function) */
constexpr static LatexTokenChild nthDiffToken[] = {
    {.leftDelimiter = "\\frac{d^{", .indexInLayout = Derivative::k_orderIndex},
    {.leftDelimiter = "}}{d", .indexInLayout = Derivative::k_variableIndex},
    {.leftDelimiter = "^{", .indexInLayout = Derivative::k_orderIndex},
    // Split '}}' and the parentheses because the latter are optional
    {.leftDelimiter = "}}", .indexInLayout = k_noChild},
    {.leftDelimiter = "\\left(", .indexInLayout = Derivative::k_derivandIndex},
    {.leftDelimiter = "\\right)", .indexInLayout = k_noChild}};

/* Latex:
 * \\frac{d^{\Order}}{d\Variable^{\Order}}(\Function)_{\Variable=\Abscissa}
 * Layout: Diff(\Variable, \Abscissa, \Order, \Function) */
constexpr static LatexTokenChild nthDiffAtAToken[] = {
    {.leftDelimiter = "\\frac{d^{", .indexInLayout = Derivative::k_orderIndex},
    {.leftDelimiter = "}}{d", .indexInLayout = Derivative::k_variableIndex},
    {.leftDelimiter = "^{", .indexInLayout = Derivative::k_orderIndex},
    {.leftDelimiter = "}}\\left(",
     .indexInLayout = Derivative::k_derivandIndex},
    {.leftDelimiter = "\\right)_{",
     .indexInLayout = Derivative::k_variableIndex},
    {.leftDelimiter = "=", .indexInLayout = Derivative::k_abscissaIndex},
    {.leftDelimiter = "}", .indexInLayout = k_noChild}};

// Code points
constexpr static LatexTokenChild middleDotToken[] = {
    {.leftDelimiter = "\\cdot", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild multiplicationSignToken[] = {
    {.leftDelimiter = "\\times", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild lesserOrEqualToken[] = {
    {.leftDelimiter = "\\le", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild greaterOrEqualToken[] = {
    {.leftDelimiter = "\\ge", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild degreeToken[] = {
    {.leftDelimiter = "\\degree", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild rightwardsArrowToken[] = {
    {.leftDelimiter = "\\to", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild infinityToken[] = {
    {.leftDelimiter = "\\infty", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild divisionToken[] = {
    {.leftDelimiter = "\\div", .indexInLayout = k_noChild}};

// Tokens that do nothing
constexpr static LatexTokenChild textToken[] = {
    {.leftDelimiter = "\\text{", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild operatorToken[] = {
    {.leftDelimiter = "\\operatorname{", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild spaceToken[] = {
    {.leftDelimiter = " ", .indexInLayout = k_noChild}};
/* TODO: Currently we are working with MathQuill which doesn't recognize the
 * special characters spacings. See
 * https://github.com/desmosinc/mathquill/blob/f71f190ee067a9a2a33683cdb02b43333b9b240e/src/commands/math/advancedSymbols.ts#L224
 */
/* constexpr static LatexToken commaToken = {
    {.leftDelimiter = ",", .indexInLayout = k_noChild}}; */
constexpr static LatexTokenChild escapeToken[] = {
    {.leftDelimiter = "\\", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild leftBraceToken[] = {
    {.leftDelimiter = "{", .indexInLayout = k_noChild}};
constexpr static LatexTokenChild rightBraceToken[] = {
    {.leftDelimiter = "}", .indexInLayout = k_noChild}};

using LayoutDetector = bool (*)(const Tree*);
using EmptyLayoutBuilder = Tree* (*)();
using LayoutCustomParserBuilder = Tree* (*)(const char**);

struct LatexLayoutRule {
  /* The latex token. Is used to:
   * - detect a latex token when turning Latex to Layout
   * - build a latex string when turning Layout to Latex
   * */
  const LatexToken latexToken;
  const int latexTokenSize;
  // Detect if a layout should be turned into this latex token
  const LayoutDetector detectLayout;
  // Builds a layout from this latex token (default method)
  const EmptyLayoutBuilder buildEmptyLayout;
  // Builds a layout from this latex token (custom method)
  const LayoutCustomParserBuilder customParseAndBuildLayout = nullptr;
};

#define ONE_CHILD_RULE(LATEX_TOKEN, IS_LAYOUT, KTREE)          \
  {                                                            \
    LATEX_TOKEN, std::size(LATEX_TOKEN),                       \
        [](const Tree* t) -> bool { return t->IS_LAYOUT(); },  \
        []() -> Tree* { return KTREE(KRackL())->cloneTree(); } \
  }

#define TWO_CHILDREN_RULE(LATEX_TOKEN, IS_LAYOUT, KTREE)                 \
  {                                                                      \
    LATEX_TOKEN, std::size(LATEX_TOKEN),                                 \
        [](const Tree* t) -> bool { return t->IS_LAYOUT(); },            \
        []() -> Tree* { return KTREE(KRackL(), KRackL())->cloneTree(); } \
  }

#define CODEPOINT_RULE(LATEX_TOKEN, CODEPOINT)                          \
  {                                                                     \
    LATEX_TOKEN, std::size(LATEX_TOKEN),                                \
        [](const Tree* t) -> bool {                                     \
          return CodePointLayout::IsCodePoint(t, CODEPOINT);            \
        },                                                              \
        []() -> Tree* { return KCodePointL<CODEPOINT>()->cloneTree(); } \
  }

#define DO_NOTHING_RULE(LATEX_TOKEN)                 \
  {                                                  \
    LATEX_TOKEN, std::size(LATEX_TOKEN),             \
        [](const Tree* t) -> bool { return false; }, \
        []() -> Tree* { return nullptr; }            \
  }

Tree* CustomParseAndBuildIntegralLayout(const char** start);
bool IsDerivativeLayout(const Tree* l, bool withoutOrder, bool withoutVariable);

constexpr static LatexLayoutRule k_rules[] = {
    // Parenthesis
    ONE_CHILD_RULE(parenthesisToken, isParenthesesLayout, KParenthesesL),
    // Curly braces
    ONE_CHILD_RULE(curlyBracesToken, isCurlyBracesLayout, KCurlyBracesL),
    // Absolute value
    ONE_CHILD_RULE(absToken, isAbsLayout, KAbsL),
    // Sqrt
    ONE_CHILD_RULE(sqrtToken, isSqrtLayout, KSqrtL),
    // Conjugate
    ONE_CHILD_RULE(conjugateToken, isConjLayout, KConjL),
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
    // Root
    TWO_CHILDREN_RULE(nthRootToken, isRootLayout, KRootL),
    // Binomial
    TWO_CHILDREN_RULE(binomToken, isBinomialLayout, KBinomialL),

    // Integral
    {integralToken, std::size(integralToken),
     [](const Tree* l) -> bool { return l->isIntegralLayout(); }, nullptr,
     CustomParseAndBuildIntegralLayout},
    // Sum
    // TODO: make parentheses optional
    {sumToken, std::size(sumToken),
     [](const Tree* l) -> bool { return l->isSumLayout(); },
     []() -> Tree* {
       return KSumL(KRackL(), KRackL(), KRackL(), KRackL())->cloneTree();
     }},
    // Product
    // TODO: make parentheses optional
    {prodToken, std::size(prodToken),
     [](const Tree* l) -> bool { return l->isProductLayout(); },
     []() -> Tree* {
       return KProductL(KRackL(), KRackL(), KRackL(), KRackL())->cloneTree();
     }},

    /* WARNING: The order matters here
     * Diff layouts need to stay is this order and be before "frac" to be
     * detected correctly */
    // Diff
    // TODO: make parentheses optional
    // TODO: fill abscissa with variable
    {diffToken, std::size(diffToken),
     [](const Tree* l) -> bool { return IsDerivativeLayout(l, false, false); },
     []() -> Tree* {
       return KDiffL(KRackL(), KRackL(), "1"_l, KRackL())->cloneTree();
     }},
    // Diff at A
    // TODO: do not fill variable twice + raise if variables are different
    {diffAtAToken, std::size(diffAtAToken),
     [](const Tree* l) -> bool { return IsDerivativeLayout(l, false, true); },
     []() -> Tree* {
       return KDiffL(KRackL(), KRackL(), "1"_l, KRackL())->cloneTree();
     }},
    // Nth diff
    // TODO: make parentheses optional
    // TODO: fill abscissa with variable
    // TODO: do not fill order twice + raise if orders are different
    // TODO: properly detect. do not mix with "d^2/3" for example
    {nthDiffToken, std::size(nthDiffToken),
     [](const Tree* l) -> bool { return IsDerivativeLayout(l, true, false); },
     []() -> Tree* {
       return KNthDiffL(KRackL(), KRackL(), KRackL(), KRackL())->cloneTree();
     }},
    // Nth diff at A
    // TODO: do not fill order twice + raise if orders are different
    // TODO: do not fill variable twice + raise if variables are different
    {nthDiffAtAToken, std::size(nthDiffAtAToken),
     [](const Tree* l) -> bool { return IsDerivativeLayout(l, true, true); },
     []() -> Tree* {
       return KNthDiffL(KRackL(), KRackL(), KRackL(), KRackL())->cloneTree();
     }},
    // Fraction
    TWO_CHILDREN_RULE(fracToken, isFractionLayout, KFracL),

    /* WARNING: The order matters here, since we want "\left(" to be checked
     * before "\le" */
    // Middle Dot
    CODEPOINT_RULE(middleDotToken, UCodePointMiddleDot),
    // Multiplication sign
    CODEPOINT_RULE(multiplicationSignToken, UCodePointMultiplicationSign),
    // <=
    CODEPOINT_RULE(lesserOrEqualToken, UCodePointInferiorEqual),
    // >=
    CODEPOINT_RULE(greaterOrEqualToken, UCodePointSuperiorEqual),
    // °
    CODEPOINT_RULE(degreeToken, UCodePointDegreeSign),
    // ->
    CODEPOINT_RULE(rightwardsArrowToken, UCodePointRightwardsArrow),
    // Infinity
    CODEPOINT_RULE(infinityToken, UCodePointInfinity),
    // ÷
    {divisionToken, std::size(divisionToken),
     // This codepoint doesn't exist in Poincare
     [](const Tree* t) -> bool { return false; },
     []() -> Tree* { return KCodePointL<'/'>()->cloneTree(); }},
    // Tokens that do nothing
    DO_NOTHING_RULE(textToken),
    DO_NOTHING_RULE(operatorToken),
    DO_NOTHING_RULE(spaceToken),
    // DO_NOTHING_RULE(commaToken),
    DO_NOTHING_RULE(escapeToken),
    DO_NOTHING_RULE(leftBraceToken),
    DO_NOTHING_RULE(rightBraceToken),
};

// ===== Latex to Layout ======

Tree* NextLatexToken(const char** start);

void ParseLatexOnRackUntilIdentifier(Rack* parent, const char** start,
                                     const char* endIdentifier) {
  size_t endLen = strlen(endIdentifier);

  bool ignoreEndIdentifier = endLen == 0;

  while (**start != 0 &&
         (ignoreEndIdentifier || strncmp(*start, endIdentifier, endLen) != 0)) {
    Tree* child = NextLatexToken(start);
    if (child) {
      NAry::AddChild(parent, child);
    }
  }

  if (ignoreEndIdentifier) {
    return;
  }

  if (**start == 0) {
    /* We're at the end of the string and endIdentifier couldn't be found */
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  *start += endLen;
}

Tree* NextLatexToken(const char** start) {
  for (const LatexLayoutRule& rule : k_rules) {
    const LatexToken latexToken = rule.latexToken;
    const char* leftDelimiter = latexToken[0].leftDelimiter;
    size_t leftDelimiterLength = strlen(leftDelimiter);
    if (strncmp(*start, leftDelimiter, leftDelimiterLength) != 0) {
      continue;
    }
    // Token found
    *start += leftDelimiterLength;
    if (rule.customParseAndBuildLayout) {
      return rule.customParseAndBuildLayout(start);
    }

    Tree* layoutToken = rule.buildEmptyLayout();

    // Parse children
    for (int i = 0; i < rule.latexTokenSize - 1; i++) {
      const char* rightDelimiter = latexToken[i + 1].leftDelimiter;
      int indexInLayout = latexToken[i].indexInLayout;
      if (indexInLayout == k_noChild) {
        int rightDelimiterLength = strlen(rightDelimiter);
        if (strncmp(*start, rightDelimiter, rightDelimiterLength) != 0) {
          TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
        }
        *start += rightDelimiterLength;
        continue;
      }
      assert(indexInLayout >= 0 &&
             indexInLayout < layoutToken->numberOfChildren());
      ParseLatexOnRackUntilIdentifier(
          Rack::From(layoutToken->child(indexInLayout)), start, rightDelimiter);
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
    ParseLatexOnRackUntilIdentifier(Rack::From(result), &latexString, "");
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

    bool ruleFound = false;
    for (const LatexLayoutRule& rule : k_rules) {
      if (!rule.detectLayout(child)) {
        continue;
      }

      const LatexToken latexToken = rule.latexToken;
      ruleFound = true;
      bool isLatexCodePoint = latexToken[0].indexInLayout == k_noChild;

      for (int i = 0; i < rule.latexTokenSize; i++) {
        // Write delimiter
        const char* leftDelimiter = latexToken[i].leftDelimiter;
        size_t leftDelimiterLength = strlen(leftDelimiter);

        if (buffer + leftDelimiterLength + isLatexCodePoint >= end) {
          // Buffer is too short
          TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
        }
        memcpy(buffer, leftDelimiter, leftDelimiterLength);
        buffer += leftDelimiterLength;

        // Write child
        int indexInLayout = latexToken[i].indexInLayout;
        if (indexInLayout != k_noChild) {
          buffer =
              serializer(Rack::From(child->child(indexInLayout)), buffer, end);
        }
      }

      if (isLatexCodePoint) {
        /* Add a space after latex codepoints, otherwise the string might
         * not be valid in latex.
         * 3\cdotcos -> not valid
         * 3\cdot cos -> valid
         **/
        *buffer = ' ';
        buffer += 1;
      }
      *buffer = 0;
    }

    if (ruleFound) {
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

// ===== Custom constructors =====

// Helper. Measures the char length of a token that contains multiple codepoints
size_t TokenCharLength(Token token) {
  LayoutSpan span = token.toSpan();
  size_t result = 0;
  LayoutSpanDecoder decoder(span);
  while (!decoder.isEmpty()) {
    CodePoint c = decoder.nextCodePoint();
    if (c == UCodePointNull) {
      break;
    }
    result += UTF8Decoder::CharSizeOfCodePoint(c);
  }
  return result;
}

/* It's no that easy to know where the integral ends in Latex, as there is no
 * clear delimiter between the integrand and the symbol, and at the end of the
 * symbol.
 *
 * Also it's not clear if "\\int_{0}^{1}ta^{3}dta" is
 * - "int(ta, 0, 1, (ta)^3)"
 * - "int(t, 0, 1, t * a^3) * a".
 * Desmos chose the second option as they don't accept variables with
 * multiple characters. But Poincaré does accept such variables, so we must
 * implement the first option. This is mandatory because the parsing must
 * work both ways, i.e. a layout converted to latex must be convertable back
 * to the same layout.
 *
 * For the delimiter between the integrand and the symbol, we need the find
 * a 'd' inside the integral.
 * But we can't just look for the first 'd' in the integrand, as it could be
 * part of another identifier like "undef" or "round". We thus use the tokenizer
 * to find the first 'd' that is not part of another identifier.
 *
 * For the delimiter at the end of the symbol, the parsing will stop as soon as
 * a non-"IdentifierMaterial" codepoint is found (i.e. non-alphanumeric
 *codepoint or greek letter. See Tokenizer::IsIdentifierMaterial for more info).
 **/
Tree* CustomParseAndBuildIntegralLayout(const char** start) {
  Tree* result =
      KIntegralL(KRackL(), KRackL(), KRackL(), KRackL())->cloneTree();

  constexpr static int k_integrandIndexInToken = 2;
  constexpr static int k_variableIndexInToken = 3;

  // --- Step 1 --- Parse upper and lower bounds in a classic way
  for (int i = 0; i < k_integrandIndexInToken; i++) {
    int indexInLayout = integralToken[i].indexInLayout;
    const char* rightDelimiter = integralToken[i + 1].leftDelimiter;
    ParseLatexOnRackUntilIdentifier(Rack::From(result->child(indexInLayout)),
                                    start, rightDelimiter);
  }

  // --- Step 2 --- Parse integrand
  /* The difficulty here is to know where the integrand ends.
   * The integral should end with `d`+variable, but  we can't just look for
   * the first `d` in the integrand as it could be part of another identifier
   * like "undef" ou "round". */
  Rack* integrandRack = Rack::From(
      result->child(integralToken[k_integrandIndexInToken].indexInLayout));
  const char* integrandStart = *start;

  EmptyContext emptyContext;
  ParsingContext parsingContext(&emptyContext,
                                ParsingContext::ParsingMethod::Classic);
  while (**start != 0) {
    if (**start == 'd') {
      // We might have found the end of the integrand
      const char* dPosition = *start;

      // --- Step 2.1 --- Find the identifier string surrounding the `d`
      /* Example: ∫3+abcdxyz1
       *                ↑ Start is at 'd'
       *             ↑------↑ Surrounding identifier string is 'abcdxyz1'
       * */
      UTF8Decoder decoderStart(integrandStart - 1, dPosition);
      CodePoint previousCodePoint = decoderStart.previousCodePoint();
      while (decoderStart.stringPosition() >= integrandStart &&
             Tokenizer::IsIdentifierMaterial(previousCodePoint) &&
             !previousCodePoint.isDecimalDigit()) {
        previousCodePoint = decoderStart.previousCodePoint();
      }
      decoderStart.nextCodePoint();
      const char* identifierStart = decoderStart.stringPosition();

      UTF8Decoder decoderEnd(dPosition);
      CodePoint nextCodePoint = decoderEnd.nextCodePoint();
      while (Tokenizer::IsIdentifierMaterial(nextCodePoint)) {
        nextCodePoint = decoderEnd.nextCodePoint();
      }
      decoderEnd.previousCodePoint();
      const char* identifierEnd = decoderEnd.stringPosition();

      // --- Step 2.2 --- Tokenize the identifier string until the 'd'
      /* Examples:
       * "abcdxyz1" -> "a" "b" "c" "d" ...
       *            -> The 'd' is not part of another identifier
       *            -> It's the delimiter
       * "abundefz" -> "a" "b" "undef" ...
       *            -> The 'd' is part of "undef"
       *            -> It's not the delimiter
       */
      Rack* rack = RackFromText(identifierStart, identifierEnd);

      Tokenizer tokenizer(rack, &parsingContext);
      Token currentToken;
      size_t totalTokensLength = 0;
      while (dPosition >= identifierStart + totalTokensLength) {
        currentToken = tokenizer.popToken();
        size_t tokenLength = TokenCharLength(currentToken);
        totalTokensLength += tokenLength;
      }
      Token nextToken = tokenizer.popToken();
      rack->removeTree();

      /* currentToken contains the 'd'
       * If it's a custom identifier and has length 1, it's the delimiter.
       * We also check that there is a following token that can be used as
       * variable.
       * */
      if (currentToken.type() == Token::Type::CustomIdentifier &&
          currentToken.length() == 1 &&
          nextToken.type() != Token::Type::EndOfStream) {
        break;
      }
    }

    // Parse the content of the integrand
    Tree* child = NextLatexToken(start);
    if (child) {
      NAry::AddChild(integrandRack, child);
    }
  }

  if (**start == 0) {
    /* We're at the end of the string and the 'd' couldn't be found */
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  // Skip 'd'
  *start += 1;

  // --- Step 3 --- Parse variable
  UTF8Decoder decoder(*start);
  CodePoint c = decoder.nextCodePoint();
  while (Tokenizer::IsIdentifierMaterial(c)) {
    Tree* codepoint = CodePointLayout::Push(c);
    NAry::AddChild(Rack::From(result->child(
                       integralToken[k_variableIndexInToken].indexInLayout)),
                   codepoint);
    c = decoder.nextCodePoint();
  }
  decoder.previousCodePoint();
  *start = decoder.stringPosition();
  return result;
}

bool IsDerivativeLayout(const Tree* l, bool isNthDerivative, bool hasAbscissa) {
  return l->isDiffLayout() &&
         (isNthDerivative == l->toDiffLayoutNode()->isNthDerivative) &&
         (hasAbscissa ||
          l->child(Derivative::k_variableIndex)
              ->treeIsIdenticalTo(l->child(Derivative::k_abscissaIndex)));
}

}  // namespace LatexParser

}  // namespace Poincare::Internal
