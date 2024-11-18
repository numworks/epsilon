#include "latex_parser.h"

#include <omg/utf8_decoder.h>
#include <poincare/old/empty_context.h>
#include <poincare/src/layout/code_point_layout.h>
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
constexpr static const char* conjugateToken[] = {"\\overline{", "\0", "}"};
constexpr static const char* superscriptToken[] = {"^{", "\0", "}"};
constexpr static const char* subscriptToken[] = {"_{", "\0", "}"};
constexpr static const char* fracToken[] = {"\\frac{", "\0", "}{", "\1", "}"};
// The root's power is at index 0 in latex and 1 in layouts
constexpr static const char* nthRootToken[] = {"\\sqrt[", "\1", "]{", "\0",
                                               "}"};
constexpr static const char* binomToken[] = {"\\binom{", "\0", "}{", "\1", "}"};

/* Latex: \\int_{\LowerBound}^{\UpperBound}\Integrand d\Symbol
 * Layout: Integral(\Symbol, \LowerBound, \UpperBound, \Integrand)
 * Custom parser. See CustomParseAndBuildIntegralLayout implementation below
 * */
constexpr static const char* integralToken[] = {
    "\\int_{", "\1", "}^{", "\2", "}", "\3", "\\ d", "\0", " "};
Tree* CustomParseAndBuildIntegralLayout(const char** start);

// Code points
constexpr static const char* middleDotToken[] = {"\\cdot"};
constexpr static const char* multiplicationSignToken[] = {"\\times"};
constexpr static const char* lesserOrEqualToken[] = {"\\le"};
constexpr static const char* greaterOrEqualToken[] = {"\\ge"};
constexpr static const char* degreeToken[] = {"\\degree"};
constexpr static const char* rightwardsArrowToken[] = {"\\to"};
constexpr static const char* infinityToken[] = {"\\infty"};
constexpr static const char* divisionToken[] = {"\\div"};

// Tokens that do nothing
constexpr static const char* textToken[] = {"\\text{"};
constexpr static const char* operatorToken[] = {"\\operatorname{"};
constexpr static const char* spaceToken[] = {" "};
/* TODO: Currently we are working with MathQuill which doesn't recognize the
 * special characters spacings. See
 * https://github.com/desmosinc/mathquill/blob/f71f190ee067a9a2a33683cdb02b43333b9b240e/src/commands/math/advancedSymbols.ts#L224
 */
// constexpr static const char* commaToken[] = {","};
constexpr static const char* escapeToken[] = {"\\"};
constexpr static const char* leftBraceToken[] = {"{"};
constexpr static const char* rightBraceToken[] = {"}"};

using LayoutDetector = bool (*)(const Tree*);
using EmptyLayoutBuilder = Tree* (*)();
using LayoutCustomParserBuilder = Tree* (*)(const char**);

struct LatexToken {
  const char* const* description;
  const int descriptionLength;
  const LayoutDetector detector;
  const EmptyLayoutBuilder buildEmptyLayout;
  const LayoutCustomParserBuilder customParseAndBuildLayout = nullptr;
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
    // Conjugate
    ONE_CHILD_TOKEN(conjugateToken, isConjLayout, KConjL),
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
     [](const Tree* l) -> bool { return l->isIntegralLayout(); }, nullptr,
     CustomParseAndBuildIntegralLayout},
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
    // °
    CODEPOINT_TOKEN(degreeToken, UCodePointDegreeSign),
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
    // DO_NOTHING_TOKEN(commaToken),
    DO_NOTHING_TOKEN(escapeToken),
    DO_NOTHING_TOKEN(leftBraceToken),
    DO_NOTHING_TOKEN(rightBraceToken),
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
  for (const LatexToken& token : k_tokens) {
    const char* leftDelimiter = token.description[0];
    size_t leftDelimiterLength = strlen(leftDelimiter);
    if (strncmp(*start, leftDelimiter, leftDelimiterLength) != 0) {
      continue;
    }
    // Token found
    *start += leftDelimiterLength;
    if (token.customParseAndBuildLayout) {
      return token.customParseAndBuildLayout(start);
    }

    Tree* layoutToken = token.buildEmptyLayout();

    // Parse children
    for (int i = 1; i < token.descriptionLength - 1; i += 2) {
      assert(strlen(token.description[i]) <= 1);
      int childIndexInLayout = token.description[i][0];
      const char* rightDelimiter = token.description[i + 1];
      ParseLatexOnRackUntilIdentifier(
          Rack::From(layoutToken->child(childIndexInLayout)), start,
          rightDelimiter);
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

  constexpr int k_boundsIndex[] = {1, 3};
  constexpr int k_integrandIndex = 5;
  constexpr int k_variableIndex = 7;

  // --- Step 1 --- Parse upper and lower bounds in a classic way
  for (int i = 0; i < 2; i++) {
    int index = k_boundsIndex[i];
    int childIndexInLayout = integralToken[index][0];
    const char* rightDelimiter = integralToken[index + 1];
    ParseLatexOnRackUntilIdentifier(
        Rack::From(result->child(childIndexInLayout)), start, rightDelimiter);
  }

  // --- Step 2 --- Parse integrand
  /* The difficulty here is to know where the integrand ends.
   * The integral should end with `d`+variable, but  we can't just look for
   * the first `d` in the integrand as it could be part of another identifier
   * like "undef" ou "round". */
  Rack* integrandRack =
      Rack::From(result->child(integralToken[k_integrandIndex][0]));
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
        totalTokensLength -= tokenLength;
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
    NAry::AddChild(Rack::From(result->child(integralToken[k_variableIndex][0])),
                   codepoint);
    c = decoder.nextCodePoint();
  }
  decoder.previousCodePoint();
  *start = decoder.stringPosition();
  return result;
}

}  // namespace LatexParser

}  // namespace Poincare::Internal
