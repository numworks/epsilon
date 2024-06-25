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

// ===== Tokens =====

/* These token description arrays alternate
 *  - A delimiter string (ex: "\\left(" or "\\right)")
 *  - A string containing 1 char that matches the index of the child in the
 * layout (ex: "\0" or "\1")
 * */
constexpr static const char* parenthesisToken[] = {"\\left(", "\0", "\\right)"};
constexpr static const char* curlyBracesToken[] = {"\\left{", "\0", "\\right}"};
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
 * We rely on the fact that the user makes it end with " dt"
 *  Layout: Integral(\Symbol, \LowerBound, \UpperBound, \Integrand)
 *  Latex: int_{\LowerBound}^{\UpperBound}\Integrand\ dt
 * */
constexpr static const char* integralToken[] = {"\\int_{", "\1", "}^{",  "\2",
                                                "}",       "\3", "\\ dt"};
constexpr static const char* middleDotToken[] = {"\\cdot"};

using LayoutDetector = bool (*)(const Tree*);
using LayoutConstructor = Tree* (*)();

struct LatexToken {
  const char* const* description;
  const int descriptionLength;
  const LayoutDetector detector;
  const LayoutConstructor constructor;
};

constexpr static LatexToken k_tokens[] = {
    // Middle Dot
    {middleDotToken, std::size(middleDotToken),
     [](const Tree* t) -> bool {
       return t->isCodePointLayout() &&
              CodePointLayout::GetCodePoint(t) == UCodePointMiddleDot;
     },
     []() -> Tree* { return KCodePointL<UCodePointMiddleDot>()->cloneTree(); }},
    // Parenthesis
    {parenthesisToken, std::size(parenthesisToken),
     [](const Tree* t) -> bool { return t->isParenthesesLayout(); },
     []() -> Tree* { return KParenthesesL(KRackL())->cloneTree(); }},
    // Curly braces
    {curlyBracesToken, std::size(curlyBracesToken),
     [](const Tree* t) -> bool { return t->isCurlyBracesLayout(); },
     []() -> Tree* { return KCurlyBracesL(KRackL())->cloneTree(); }},
    // Absolute value
    {absToken, std::size(absToken),
     [](const Tree* t) -> bool { return t->isAbsLayout(); },
     []() -> Tree* { return KAbsL(KRackL())->cloneTree(); }},
    // Sqrt
    {sqrtToken, std::size(sqrtToken),
     [](const Tree* t) -> bool { return t->isSqrtLayout(); },
     []() -> Tree* { return KSqrtL(KRackL())->cloneTree(); }},
    // Superscript
    {superscriptToken, std::size(superscriptToken),
     [](const Tree* t) -> bool {
       return t->isVerticalOffsetLayout() && VerticalOffset::IsSuperscript(t);
     },
     // Subscript
     []() -> Tree* { return KSuperscriptL(KRackL())->cloneTree(); }},
    {subscriptToken, std::size(subscriptToken),
     [](const Tree* t) -> bool {
       return t->isVerticalOffsetLayout() && VerticalOffset::IsSubscript(t);
     },
     []() -> Tree* { return KSubscriptL(KRackL())->cloneTree(); }},
    // Fraction
    {fracToken, std::size(fracToken),
     [](const Tree* t) -> bool { return t->isFractionLayout(); },
     []() -> Tree* { return KFracL(KRackL(), KRackL())->cloneTree(); }},
    // Root
    {nthRootToken, std::size(nthRootToken),
     [](const Tree* t) -> bool { return t->isRootLayout(); },
     []() -> Tree* { return KRootL(KRackL(), KRackL())->cloneTree(); }},
    // Binomial
    {binomToken, std::size(binomToken),
     [](const Tree* t) -> bool { return t->isBinomialLayout(); },
     []() -> Tree* { return KBinomialL(KRackL(), KRackL())->cloneTree(); }},
    // Integral
    // For now, you can only integrate in x
    {integralToken, std::size(integralToken),
     [](const Tree* t) -> bool { return t->isIntegralLayout(); },
     []() -> Tree* {
       return KIntegralL("t"_l, KRackL(), KRackL(), KRackL())->cloneTree();
     }},
};

// ===== Latex to Layout ======

Tree* NextLatexToken(const char** start);

void ParseLatexOnRackUntilIdentifier(Rack* parent, const char** start,
                                     const char* endIdentifier) {
  size_t endLen = strlen(endIdentifier);
  while (**start != 0 &&
         (endLen == 0 || strncmp(*start, endIdentifier, endLen) != 0)) {
    Tree* child = NextLatexToken(start);
    if (child) {
      NAry::AddChild(parent, child);
    }
  }

  if (**start == 0 && endLen > 0) {
    // endIdentifier couldn't be found
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
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
    for (int i = 1; i < token.descriptionLength; i += 2) {
      assert(strlen(token.description[i]) <= 1);
      int childIndexInLayout = token.description[i][0];
      const char* rightDelimiter = token.description[i + 1];
      ParseLatexOnRackUntilIdentifier(
          Rack::From(layoutToken->child(childIndexInLayout)), start,
          rightDelimiter);
      *start += strlen(rightDelimiter);
    }

    return layoutToken;
  }

  if (**start == '\\' || **start == ' ' || **start == '{' || **start == '}') {
    /* Ignore \ and {} if it doesn't belong to a known token
     * Ignore spaces */
    *start += 1;
    return nullptr;
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
 *   ThousandSeparator -> replaced with ' '
 *
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

char* LayoutToLatexWithExceptions(const Rack* rack, char* buffer, char* end) {
  for (const Tree* child : rack->children()) {
    if (child->isOperatorSeparatorLayout()) {
      // Invisible in latex
      continue;
    }

    if (buffer >= end) {
      break;
    }

    if (child->isCodePointLayout()) {
      buffer = CodePointLayout::CopyName(child, buffer, end - buffer);
      continue;
    }

    if (child->isThousandSeparatorLayout()) {
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
      while (true) {
        const char* delimiter = token.description[i];
        size_t delimiterLength = strlen(delimiter);
        if (buffer + delimiterLength >= end) {
          // Buffer is too short
          TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
        }
        memcpy(buffer, delimiter, delimiterLength);
        buffer += delimiterLength;
        if (i == token.descriptionLength - 1) {
          *buffer = 0;
          break;
        }
        assert(strlen(token.description[i + 1]) <= 1);
        int indexOfChildInLayout = token.description[i + 1][0];
        buffer = LayoutToLatexWithExceptions(
            Rack::From(child->child(indexOfChildInLayout)), buffer, end);
        i += 2;
      }
      tokenFound = true;
    }

    if (tokenFound) {
      continue;
    }

    // Use common serialization
    buffer = SerializeLayout(Layout::From(child), buffer, end,
                             LayoutToLatexWithExceptions);
    *buffer = 0;
  }

  if (buffer >= end) {
    // Buffer is too short
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }

  return buffer;
}

char* LayoutToLatex(const Rack* rack, char* buffer, char* end) {
  ExceptionTry {
    char* result = LayoutToLatexWithExceptions(rack, buffer, end);
    return result;
  }
  ExceptionCatch(type) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type);
    }
    *buffer = 0;
    return buffer;
  }
}

}  // namespace LatexParser

}  // namespace Poincare::Internal
