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
 * {{ "\\sqrt[",  1},
 *  { "]{",  0},
 *  { "}",  k_noChild}};
 * */

struct LatexTokenChild {
  const char* leftDelimiter;
  int indexInLayout;
};

const static int k_noChild = -1;

using LatexToken = const LatexTokenChild*;

constexpr static LatexTokenChild parenthesisToken[] = {{"\\left(", 0},
                                                       {"\\right)", k_noChild}};
constexpr static LatexTokenChild curlyBracesToken[] = {
    {"\\left\\{", 0}, {"\\right\\}", k_noChild}};

constexpr static LatexTokenChild absToken[] = {{"\\left|", 0},
                                               {"\\right|", k_noChild}};

constexpr static LatexTokenChild sqrtToken[] = {{"\\sqrt{", 0},
                                                {"}", k_noChild}};

constexpr static LatexTokenChild conjugateToken[] = {{"\\overline{", 0},
                                                     {"}", k_noChild}};

constexpr static LatexTokenChild superscriptToken[] = {{"^{", 0},
                                                       {"}", k_noChild}};

constexpr static LatexTokenChild subscriptToken[] = {{"_{", 0},
                                                     {"}", k_noChild}};

constexpr static LatexTokenChild fracToken[] = {
    {"\\frac{", Fraction::k_numeratorIndex},
    {"}{", Fraction::k_denominatorIndex},
    {"}", k_noChild}};

constexpr static LatexTokenChild nthRootToken[] = {
    {"\\sqrt[", NthRoot::k_indexIndex},
    {"]{", NthRoot::k_radicandIndex},
    {"}", k_noChild}};

constexpr static LatexTokenChild binomToken[] = {
    {"\\binom{", Binomial::k_nIndex},
    {"}{", Binomial::k_kIndex},
    {"}", k_noChild}};

/* Latex: \\int_{\LowerBound}^{\UpperBound}\Integrand d\Symbol
 * Layout: Integral(\Symbol, \LowerBound, \UpperBound, \Integrand) */
constexpr static LatexTokenChild integralToken[] = {
    {"\\int_{", Integral::k_lowerBoundIndex},
    {"}^{", Integral::k_upperBoundIndex},
    // Split '}' and the integrand because the latter has custom parsing
    {"}", k_noChild},
    {"", Integral::k_integrandIndex},
    {"\\ d", Integral::k_differentialIndex},
    /* Add a space to separate the variable from the next latex token.
     * This space is not visible to the user in latex (" " != "\\ " ) */
    {" ", k_noChild}};
constexpr static int k_integrandIndexInIntegralToken = 3;
constexpr static int k_variableIndexInIntegralToken = 4;
constexpr static int k_endIndexInIntegralToken = 5;

/* Latex: \\sum_{\Variable=\LowerBound}^{\UpperBound}(\Function)
 * Layout: Sum(\Variable, \LowerBound, \UpperBound, \Function) */
constexpr static LatexTokenChild sumToken[] = {
    {"\\sum_{", Parametric::k_variableIndex},
    {"=", Parametric::k_lowerBoundIndex},
    {"}^{", Parametric::k_upperBoundIndex},
    // Split '}' and the parentheses because the latter are optional
    {"}", k_noChild},
    {"\\left(", Parametric::k_argumentIndex},
    {"\\right)", k_noChild}};

/* Latex: \\prod{\Variable=\LowerBound}^{\UpperBound}(\Function)
 * Layout: Product(\Variable, \LowerBound, \UpperBound, \Function) */
constexpr static LatexTokenChild prodToken[] = {
    {"\\prod_{", Parametric::k_variableIndex},
    {"=", Parametric::k_lowerBoundIndex},
    {"}^{", Parametric::k_upperBoundIndex},
    // Split '}' and the parentheses because the latter are optional
    {"}", k_noChild},
    {"\\left(", Parametric::k_argumentIndex},
    {"\\right)", k_noChild}};

/* Latex: \\frac{d}{d\Variable}(\Function)
 * Layout: Diff(\Variable, \Variable, "1"_l, \Function) */
constexpr static LatexTokenChild diffToken[] = {
    {"\\frac{d}{d", Derivative::k_variableIndex},
    // Split '}' and the parentheses because the latter are optional
    {"}", k_noChild},
    {"\\left(", Derivative::k_derivandIndex},
    {"\\right)", k_noChild}};
constexpr static int k_variableIndexInDiffToken = 0;

/* Latex: \\frac{d}{d\Variable}(\Function)_{\Variable=\Abscissa}
 * Layout: Diff(\Variable, \Abscissa, "1"_l, \Function) */
constexpr static LatexTokenChild diffAtAToken[] = {
    {"\\frac{d}{d", Derivative::k_variableIndex},
    {"}\\left(", Derivative::k_derivandIndex},
    {"\\right)_{", Derivative::k_variableIndex},
    {"=", Derivative::k_abscissaIndex},
    {"}", k_noChild}};

/* Latex: \\frac{d^{\Order}}{d\Variable^{\Order}}(\Function)
 * Layout: Diff(\Variable, \Variable, \Order, \Function) */
constexpr static LatexTokenChild nthDiffToken[] = {
    {"\\frac{d^{", Derivative::k_orderIndex},
    {"}}{d", Derivative::k_variableIndex},
    {"^{", Derivative::k_orderIndex},
    // Split '}}' and the parentheses because the latter are optional
    {"}}", k_noChild},
    {"\\left(", Derivative::k_derivandIndex},
    {"\\right)", k_noChild}};
constexpr static int k_variableIndexInNthDiffToken = 1;

/* Latex:
 * \\frac{d^{\Order}}{d\Variable^{\Order}}(\Function)_{\Variable=\Abscissa}
 * Layout: Diff(\Variable, \Abscissa, \Order, \Function) */
constexpr static LatexTokenChild nthDiffAtAToken[] = {
    {"\\frac{d^{", Derivative::k_orderIndex},
    {"}}{d", Derivative::k_variableIndex},
    {"^{", Derivative::k_orderIndex},
    {"}}\\left(", Derivative::k_derivandIndex},
    {"\\right)_{", Derivative::k_variableIndex},
    {"=", Derivative::k_abscissaIndex},
    {"}", k_noChild}};

// Code points
constexpr static LatexTokenChild middleDotToken[] = {{"\\cdot", k_noChild}};
constexpr static LatexTokenChild multiplicationSignToken[] = {
    {"\\times", k_noChild}};
constexpr static LatexTokenChild lesserOrEqualToken[] = {{"\\le", k_noChild}};
constexpr static LatexTokenChild greaterOrEqualToken[] = {{"\\ge", k_noChild}};
constexpr static LatexTokenChild degreeToken[] = {{"\\degree", k_noChild}};
constexpr static LatexTokenChild rightwardsArrowToken[] = {{"\\to", k_noChild}};
constexpr static LatexTokenChild infinityToken[] = {{"\\infty", k_noChild}};
constexpr static LatexTokenChild divisionToken[] = {{"\\div", k_noChild}};

// Tokens that do nothing
constexpr static LatexTokenChild textToken[] = {{"\\text{", 0},
                                                {"}", k_noChild}};
constexpr static LatexTokenChild operatorToken[] = {{"\\operatorname{", 0},
                                                    {"}", k_noChild}};
constexpr static LatexTokenChild spaceToken[] = {{" ", k_noChild}};
/* TODO: Currently we are working with MathQuill which doesn't recognize the
 * special characters spacings. See
 * https://github.com/desmosinc/mathquill/blob/f71f190ee067a9a2a33683cdb02b43333b9b240e/src/commands/math/advancedSymbols.ts#L224
 */
/* constexpr static LatexToken commaToken = {
    { ",",  k_noChild}}; */
constexpr static LatexTokenChild escapeToken[] = {{"\\", k_noChild}};
constexpr static LatexTokenChild systemBraceToken[] = {{"{", 0},
                                                       {"}", k_noChild}};

using LayoutDetector = bool (*)(const Tree*);
using EmptyLayoutBuilder = Tree* (*)();

struct LatexLayoutRule {
  /* The latex token. Is used to:
   * - detect a latex token when turning Latex to Layout
   * - build a latex string when turning Layout to Latex
   * */
  const LatexToken latexToken;
  const int latexTokenSize;
  // Detect if a layout should be turned into this latex token
  const LayoutDetector detectLayout;
  // Builds the layout for this latex token
  const EmptyLayoutBuilder buildEmptyLayout;
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

#define DO_NOTHING_RULE(LATEX_TOKEN)                    \
  {                                                     \
    LATEX_TOKEN, std::size(LATEX_TOKEN),                \
        [](const Tree* t) -> bool { return false; },    \
        []() -> Tree* { return KRackL()->cloneTree(); } \
  }

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
     [](const Tree* l) -> bool { return l->isIntegralLayout(); },
     []() -> Tree* {
       return KIntegralL(KRackL(), KRackL(), KRackL(), KRackL())->cloneTree();
     }},
    // Sum
    {sumToken, std::size(sumToken),
     [](const Tree* l) -> bool { return l->isSumLayout(); },
     []() -> Tree* {
       return KSumL(KRackL(), KRackL(), KRackL(), KRackL())->cloneTree();
     }},
    // Product
    {prodToken, std::size(prodToken),
     [](const Tree* l) -> bool { return l->isProductLayout(); },
     []() -> Tree* {
       return KProductL(KRackL(), KRackL(), KRackL(), KRackL())->cloneTree();
     }},

    /* WARNING: The order matters here
     * Diff layouts need to stay is this order and be before "frac" to be
     * detected correctly */
    // Diff at A
    {diffAtAToken, std::size(diffAtAToken),
     [](const Tree* l) -> bool { return IsDerivativeLayout(l, false, false); },
     []() -> Tree* {
       return KDiffL(KRackL(), KRackL(), "1"_l, KRackL())->cloneTree();
     }},
    // Diff
    {diffToken, std::size(diffToken),
     [](const Tree* l) -> bool { return IsDerivativeLayout(l, false, true); },
     []() -> Tree* {
       return KDiffL(KRackL(), KRackL(), "1"_l, KRackL())->cloneTree();
     }},
    // Nth diff at A
    {nthDiffAtAToken, std::size(nthDiffAtAToken),
     [](const Tree* l) -> bool { return IsDerivativeLayout(l, true, false); },
     []() -> Tree* {
       return KNthDiffL(KRackL(), KRackL(), KRackL(), KRackL())->cloneTree();
     }},
    // Nth diff
    {nthDiffToken, std::size(nthDiffToken),
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
    DO_NOTHING_RULE(systemBraceToken),
};

// ===== Latex to Layout ======

Tree* NextLatexToken(const char** start, const char* rightDelimiter);

void ParseLatexOnRackUntilDelimiter(Rack* rack, const char** start,
                                    const char* rightDelimiter) {
  size_t delimiterLen = strlen(rightDelimiter);
  while (**start != 0 && (delimiterLen == 0 ||
                          strncmp(*start, rightDelimiter, delimiterLen) != 0)) {
    Tree* child = NextLatexToken(start, rightDelimiter);
    if (child) {
      NAry::AddOrMergeChild(rack, child);
    }
  }
}

bool CustomBuildLayoutChildFromLatex(const char** latexString,
                                     const LatexLayoutRule& rule,
                                     int indexInLatexToken, Tree* parentLayout,
                                     const char* parentRightDelimiter);

/* This method is used to get a Rack inside a built layout and replace it with a
 * parsed child.
 * The layout is either:
 * - A normal layout, in which case the RackAtIndex is the child at index in the
 * layout.
 * - A rack layout, in which case the RackAtIndex is the rack itself and the
 * index must be 0.
 * This is useful for DO_NOTHING_RULE, where the layout generated is a Rack,
 * which should be replaced by the parsed child, instead of having its children
 * replaced.
 */
Tree* RackAtIndex(Tree* layout, int index) {
  if (layout->isRackLayout()) {
    assert(index == 0);
    return layout;
  }
  return layout->child(index);
}

Tree* NextLatexToken(const char** start, const char* rightDelimiter) {
  bool atLeastOneRuleMatched = false;

  for (const LatexLayoutRule& rule : k_rules) {
    const LatexToken latexToken = rule.latexToken;
    const char* leftDelimiter = latexToken[0].leftDelimiter;
    if (strncmp(*start, leftDelimiter, strlen(leftDelimiter)) != 0) {
      continue;
    }
    // Token found
    atLeastOneRuleMatched = true;
    const char* currentStart = *start;

    ExceptionTry {
      Tree* resultLayout = rule.buildEmptyLayout();

      // Parse children
      for (int i = 0; i < rule.latexTokenSize; i++) {
        // Check for custom parsing of child
        if (CustomBuildLayoutChildFromLatex(start, rule, i, resultLayout,
                                            rightDelimiter)) {
          continue;
        }

        // Classic parsing
        // --- Step 1. Skip left delimiter ---
        const char* childLeftDelimiter = latexToken[i].leftDelimiter;
        int childLeftDelimiterLength = strlen(childLeftDelimiter);
        if (strncmp(*start, childLeftDelimiter, childLeftDelimiterLength) !=
            0) {
          // Left delimiter not found
          TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
        }
        *start += childLeftDelimiterLength;

        // --- Step 2. Parse child ---
        int indexInLayout = latexToken[i].indexInLayout;
        if (indexInLayout == k_noChild) {
          continue;
        }
        assert((resultLayout->isRackLayout() && indexInLayout == 0) ||
               indexInLayout >= 0 &&
                   (indexInLayout < resultLayout->numberOfChildren()));
        assert(i < rule.latexTokenSize - 1);  // Last should be k_noChild
        Tree* childResult = KRackL()->cloneTree();
        const char* childRightDelimiter = latexToken[i + 1].leftDelimiter;
        ParseLatexOnRackUntilDelimiter(Rack::From(childResult), start,
                                       childRightDelimiter);

        // --- Step 3. Check if child was already parsed earlier ---
        bool wasAlreadyParsedElsewhere = false;
        for (int j = 0; j < i; j++) {
          if (latexToken[j].indexInLayout == indexInLayout) {
            wasAlreadyParsedElsewhere = true;
            if (!childResult->treeIsIdenticalTo(
                    RackAtIndex(resultLayout, indexInLayout))) {
              /* The child was already parsed elsewhere, but the result is
               * different. Thus the latex is invalid.
               * Ex: "\frac{d^{3}}{dx^{4}}x"
               *    In this n-th derivative, the first order layout is
               *    different from the second one.
               * */
              TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
            }
          }
        }
        // --- Step 4. Clone child ---
        if (!wasAlreadyParsedElsewhere) {
          RackAtIndex(resultLayout, indexInLayout)
              ->cloneTreeOverTree(childResult);
        }
      }

      return resultLayout;
    }
    /* When parsing fails, we try the next rule.
     * This is especially useful for rules like diff and frac, for which only
     * comparing the first chars is not enough to know if the rule is the right
     * one.
     * Ex: "\frac{d^{4}}{d^{2}}"
     *     This isn't an nth-derivative but just the frac d^4/d^2 */
    ExceptionCatch(type) {
      if (type != ExceptionType::ParseFail) {
        TreeStackCheckpoint::Raise(type);
      }
      *start = currentStart;
    }
  }

  // If some rule matched but the parsing still failed, there is a syntax error
  if (atLeastOneRuleMatched) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }

  // Parse as code point
  UTF8Decoder decoder(*start);
  Tree* codepoint = CodePointLayout::Push(decoder.nextCodePoint());
  *start = decoder.stringPosition();
  return codepoint;
}

Tree* LatexToLayout(const char* latexString) {
  ExceptionTry {
    Tree* result = KRackL()->cloneTree();
    ParseLatexOnRackUntilDelimiter(Rack::From(result), &latexString, "");
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
      bool isLatexCodePoint = rule.latexTokenSize == 1;

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
      break;
    }

    if (ruleFound) {
      continue;
    }

    if (child->isCodePointLayout()) {
      buffer = CodePointLayout::CopyName(child, buffer, end - buffer);
      continue;
    }

    // Use common serialization
    buffer = SerializeLayout(Layout::From(child), buffer, end,
                             rack->numberOfChildren() == 1, serializer);
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

void BuildIntegrandChildFromLatex(const char** latexString, Tree* parentLayout);

void BuildIntegralVariableChildFromLatex(const char** latexString,
                                         Tree* parentLayout);

void CloneVariableLayoutIntoAbscissaChild(const LatexLayoutRule& rule,
                                          int indexInLatexToken,
                                          Tree* parentLayout);

bool BuildChildWithOptionalParenthesesFromLatex(
    const char** latexString, const LatexLayoutRule& rule,
    int indexInLatexToken, Tree* parentLayout,
    const char* parentRightDelimiter);

bool CustomBuildLayoutChildFromLatex(const char** latexString,
                                     const LatexLayoutRule& rule,
                                     int indexInLatexToken, Tree* parentLayout,
                                     const char* parentRightDelimiter) {
  const LatexToken latexToken = rule.latexToken;

  // Integral integrand and variable
  if (latexToken == integralToken) {
    if (indexInLatexToken == k_integrandIndexInIntegralToken) {
      BuildIntegrandChildFromLatex(latexString, parentLayout);
      return true;
    } else if (indexInLatexToken == k_variableIndexInIntegralToken) {
      BuildIntegralVariableChildFromLatex(latexString, parentLayout);
      return true;
    } else if (indexInLatexToken == k_endIndexInIntegralToken) {
      // Do nothing
      return true;
    }
  }

  // Abscissa in diff and nth diff without value
  if ((latexToken == diffToken &&
       indexInLatexToken == k_variableIndexInDiffToken + 1) ||
      (latexToken == nthDiffToken &&
       indexInLatexToken == k_variableIndexInNthDiffToken + 1)) {
    // Do this just after the variable has been parsed
    CloneVariableLayoutIntoAbscissaChild(rule, indexInLatexToken, parentLayout);
    return false;
  }

  // Analysis optional parentheses
  if ((latexToken == sumToken || latexToken == prodToken ||
       latexToken == diffToken || latexToken == nthDiffToken)) {
    return BuildChildWithOptionalParenthesesFromLatex(
        latexString, rule, indexInLatexToken, parentLayout,
        parentRightDelimiter);
  }
  return false;
}

// ----- Integral -----

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
 * codepoint or greek letter. See Tokenizer::IsIdentifierMaterial for more
 * info).
 **/
void BuildIntegrandChildFromLatex(const char** latexString,
                                  Tree* parentLayout) {
  Rack* integrandRack = Rack::From(parentLayout->child(
      integralToken[k_integrandIndexInIntegralToken].indexInLayout));
  const char* integrandStart = *latexString;

  EmptyContext emptyContext;
  ParsingContext parsingContext(&emptyContext,
                                ParsingContext::ParsingMethod::Classic);
  while (**latexString != 0) {
    if (**latexString == 'd') {
      // We might have found the end of the integrand
      const char* dPosition = *latexString;

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
    Tree* child = NextLatexToken(latexString, "");
    if (child) {
      NAry::AddOrMergeChild(integrandRack, child);
    }
  }

  if (**latexString == 0) {
    /* We're at the end of the string and the 'd' couldn't be found */
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  // Skip 'd'
  *latexString += 1;
}

void BuildIntegralVariableChildFromLatex(const char** latexString,
                                         Tree* parentLayout) {
  UTF8Decoder decoder(*latexString);
  CodePoint c = decoder.nextCodePoint();
  while (Tokenizer::IsIdentifierMaterial(c)) {
    Tree* codepoint = CodePointLayout::Push(c);
    NAry::AddChild(
        Rack::From(parentLayout->child(
            integralToken[k_variableIndexInIntegralToken].indexInLayout)),
        codepoint);
    c = decoder.nextCodePoint();
  }
  decoder.previousCodePoint();
  *latexString = decoder.stringPosition();
}

bool IsDerivativeLayout(const Tree* l, bool isNthDerivative,
                        bool isAbscissaEqualToVariable) {
  /* If the abscissa layout is identical to the variable (`d/dx(...)_{x=x}`)
   * we assume the user wants to use the latex `d/dx(...)` rather than
   * `d/dx(...)_{x=a}`. This trick is necessary because no layout for `d/dx()``
   * without abscissa exists. */
  return l->isDiffLayout() &&
         (isNthDerivative == l->toDiffLayoutNode()->isNthDerivative) &&
         (isAbscissaEqualToVariable ==
          l->child(Derivative::k_variableIndex)
              ->treeIsIdenticalTo(l->child(Derivative::k_abscissaIndex)));
}

// --- Diff and NthDiff ---

/* Diff and NthDiff do not have an abscissa value provided by Latex.
 * We thus replace the abscissa with the variable. */
void CloneVariableLayoutIntoAbscissaChild(const LatexLayoutRule& rule,
                                          int indexInLatexToken,
                                          Tree* parentLayout) {
  const LatexToken latexToken = rule.latexToken;
  assert(latexToken == diffToken || latexToken == nthDiffToken);

  int variableIndex = latexToken[indexInLatexToken - 1].indexInLayout;

  Tree* variable = parentLayout->child(variableIndex);
  Tree* abscissa = parentLayout->child(Derivative::k_abscissaIndex);
  abscissa->cloneTreeOverTree(variable);
}

// ---- Analysis optional parenthesis ----

/* For Sum, Prod, diff and nthdiff, if the user doesn't use parentheses
 * around the function, we still want to be able to parse their input.
 * In this case, the algorithm considers that the function continues until
 * the end of the input (or when encountering the parent delimiter). */
bool BuildChildWithOptionalParenthesesFromLatex(
    const char** latexString, const LatexLayoutRule& rule,
    int indexInLatexToken, Tree* parentLayout,
    const char* parentRightDelimiter) {
  const LatexToken& latexToken = rule.latexToken;
  assert(latexToken == sumToken || latexToken == prodToken ||
         latexToken == diffToken || latexToken == nthDiffToken);

  if (indexInLatexToken == rule.latexTokenSize - 2) {
    const char* leftDelimiter = latexToken[indexInLatexToken].leftDelimiter;
    const char* rightDelimiter =
        latexToken[indexInLatexToken + 1].leftDelimiter;
    assert(strncmp(leftDelimiter, "\\left(", strlen("\\left(")) == 0);
    assert(strncmp(rightDelimiter, "\\right)", strlen("\\right)")) == 0);

    int leftDelimiterLength = strlen(leftDelimiter);
    bool hasLeftParenthesis =
        strncmp(*latexString, leftDelimiter, leftDelimiterLength) == 0;
    if (hasLeftParenthesis) {
      *latexString += leftDelimiterLength;
    }

    int indexInLayout = latexToken[indexInLatexToken].indexInLayout;
    assert(indexInLayout >= 0 &&
           indexInLayout < parentLayout->numberOfChildren());
    /* If no left parenthesis, parse till the end of the string
     * else, stop when reaching delimiter of parent token. */
    ParseLatexOnRackUntilDelimiter(
        Rack::From(parentLayout->child(indexInLayout)), latexString,
        hasLeftParenthesis ? rightDelimiter : parentRightDelimiter);

    // Skip right parenthesis if has left parenthesis
    if (hasLeftParenthesis) {
      int rightDelimiterLength = strlen(rightDelimiter);
      bool hasRightParenthesis =
          strncmp(*latexString, rightDelimiter, rightDelimiterLength) == 0;
      if (!hasRightParenthesis) {
        // Right parenthesis not found
        TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
      }
      *latexString += rightDelimiterLength;
    }
    return true;
  }

  if (indexInLatexToken == rule.latexTokenSize - 1) {
    // Already handled in previous child
    return true;
  }
  return false;
}

}  // namespace LatexParser

}  // namespace Poincare::Internal
