#include "xnt.h"

#include <poincare/xnt_helpers.h>
#include <poincare_junior/src/expression/parametric.h>
#include <poincare_junior/src/expression/symbol.h>

#include "k_tree.h"
#include "parsing/tokenizer.h"
#include "serialize.h"

using namespace Poincare::XNTHelpers;

namespace PoincareJ {

// Cycles
constexpr int k_maxCycleSize = 5;
constexpr CodePoint k_defaultXNTCycle[] = {
    Symbol::k_cartesianSymbol,
    Symbol::k_sequenceSymbol,
    Symbol::k_parametricSymbol,
    Symbol::k_polarSymbol,
    UCodePointNull,
};
constexpr CodePoint k_defaultContinuousXNTCycle[] = {
    Symbol::k_cartesianSymbol,
    Symbol::k_parametricSymbol,
    Symbol::k_polarSymbol,
    UCodePointNull,
};
constexpr CodePoint k_defaultDiscreteXNTCycle[] = {
    'k',
    'n',
    UCodePointNull,
};

static int indexOfCodePointInCycle(CodePoint codePoint,
                                   const CodePoint *cycle) {
  for (size_t i = 0; i < k_maxCycleSize - 1; i++) {
    if (cycle[i] == codePoint) {
      return i;
    }
  }
  assert(cycle[k_maxCycleSize - 1] == codePoint);
  return k_maxCycleSize - 1;
}

static size_t sizeOfCycle(const CodePoint *cycle) {
  return indexOfCodePointInCycle(UCodePointNull, cycle);
}

static CodePoint codePointAtIndexInCycle(int index, int startingIndex,
                                         const CodePoint *cycle,
                                         size_t *cycleSize) {
  assert(index >= 0);
  assert(cycleSize);
  *cycleSize = sizeOfCycle(cycle);
  assert(0 <= startingIndex && startingIndex < *cycleSize);
  return cycle[(startingIndex + index) % *cycleSize];
}

CodePoint CodePointAtIndexInDefaultCycle(int index, CodePoint startingCodePoint,
                                         size_t *cycleSize) {
  int startingIndex =
      indexOfCodePointInCycle(startingCodePoint, k_defaultXNTCycle);
  return codePointAtIndexInCycle(index, startingIndex, k_defaultXNTCycle,
                                 cycleSize);
}

CodePoint CodePointAtIndexInCycle(int index, const CodePoint *cycle,
                                  size_t *cycleSize) {
  return codePointAtIndexInCycle(index, 0, cycle, cycleSize);
}

// Parametered functions
constexpr struct {
  LayoutType layoutType;
  const CodePoint *XNTcycle;
} k_parameteredFunctions[] = {
    {LayoutType::Derivative, k_defaultContinuousXNTCycle},
    {LayoutType::NthDerivative, k_defaultContinuousXNTCycle},
    {LayoutType::Integral, k_defaultContinuousXNTCycle},
    {LayoutType::Sum, k_defaultDiscreteXNTCycle},
    {LayoutType::Product, k_defaultDiscreteXNTCycle},
    {LayoutType::ListSequence, k_defaultDiscreteXNTCycle},
};
constexpr int k_numberOfFunctions = std::size(k_parameteredFunctions);

constexpr int k_indexOfMainExpression1D = 0;
constexpr int k_indexOfParameter1D = 1;

bool ParameterText(UnicodeDecoder &varDecoder, size_t *parameterStart,
                   size_t *parameterLength) {
  static_assert(k_indexOfParameter1D == 1,
                "ParameteredExpression::ParameterText is outdated");
  /* Find the beginning of the parameter. Count parentheses to handle the
   * presence of functions with several parameters in the parametered
   * expression. */
  CodePoint c = UCodePointUnknown;
  bool variableFound = false;
  int cursorLevel = 0;
  while (c != UCodePointNull && cursorLevel >= 0 && !variableFound) {
    c = varDecoder.nextCodePoint();
    switch (c) {
      case '(':
      case '{':
      case '[':
      case UCodePointLeftSystemParenthesis:
        cursorLevel++;
        break;
      case ')':
      case '}':
      case ']':
      case UCodePointRightSystemParenthesis:
        cursorLevel--;
        break;
      case ',':
        // The parameter is the second argument of parametered expressions
        variableFound = cursorLevel == 0;
        break;
    }
  }
  if (!variableFound || c == UCodePointNull) {
    return false;
  }

  size_t startOfVariable = varDecoder.position();
  // Parameter name can be nested in system parentheses. Skip them
  c = varDecoder.nextCodePoint();
  bool hasSystemParenthesis = (c == UCodePointLeftSystemParenthesis);
  if (hasSystemParenthesis) {
    startOfVariable = varDecoder.position();
  }
  CodePoint previousC = UCodePointUnknown;
  while (c != UCodePointNull && c != ',' && c != ')') {
    previousC = c;
    c = varDecoder.nextCodePoint();
  }
  // Skip system parenthesis if needed
  if (hasSystemParenthesis) {
    if (previousC != UCodePointRightSystemParenthesis) {
      return false;
    }
    varDecoder.previousCodePoint();
  }
  size_t endOfVariable = varDecoder.position();
  varDecoder.unsafeSetPosition(startOfVariable);
  do {
    // Skip whitespaces
    c = varDecoder.nextCodePoint();
  } while (c == ' ');
  varDecoder.previousCodePoint();
  startOfVariable = varDecoder.position();
  size_t lengthOfVariable = endOfVariable - startOfVariable - 1;

  if (!Tokenizer::CanBeCustomIdentifier(varDecoder, lengthOfVariable)) {
    return false;
  }
  varDecoder.unsafeSetPosition(startOfVariable);
  *parameterLength = lengthOfVariable;
  *parameterStart = startOfVariable;
  return true;
}

bool ParameterText(const char *text, const char **parameterText,
                   size_t *parameterLength) {
  UTF8Decoder decoder(text);
  size_t parameterStart = *parameterText - text;
  bool result = ParameterText(decoder, &parameterStart, parameterLength);
  *parameterText = parameterStart + text;
  return result;
}

constexpr int k_indexOfParameter = Parametric::k_variableIndex;

static bool findParameteredFunction2D(const Tree *layout, int *functionIndex,
                                      int *childIndex, const Tree *root,
                                      const Tree **parameterLayout) {
  assert(functionIndex && childIndex && parameterLayout);
  *functionIndex = -1;
  *childIndex = -1;
  *parameterLayout = nullptr;
  assert(layout);
  const Tree *child = layout;
  const Tree *parent = root->parentOfDescendant(child, childIndex);
  while (parent) {
    if (parent->isParametricLayout()) {
      if (*childIndex == k_indexOfParameter ||
          *childIndex == Parametric::FunctionIndex(parent)) {
        for (size_t i = 0; i < k_numberOfFunctions; i++) {
          if (parent->layoutType() == k_parameteredFunctions[i].layoutType) {
            *functionIndex = i;
            *parameterLayout = parent->child(k_indexOfParameter);
            return true;
          }
        }
      }
    }
    child = parent;
    parent = root->parentOfDescendant(child, childIndex);
  }
  return false;
}

static bool isValidXNTParameter(const Tree *xnt) {
  for (const Tree *child : xnt->children()) {
    if (!child->isCodePointLayout()) {
      return false;
    }
  }
  RackLayoutDecoder decoder(xnt);
  if (!Tokenizer::CanBeCustomIdentifier(decoder)) {
    return false;
  }
  return true;
}

bool FindXNTSymbol2D(const Tree *layout, const Tree *root, char *buffer,
                     size_t bufferSize, int xntIndex, size_t *cycleSize) {
  assert(cycleSize);
  int functionIndex;
  int childIndex;
  const Tree *parameterLayout;
  buffer[0] = 0;
  *cycleSize = 0;
  if (findParameteredFunction2D(layout, &functionIndex, &childIndex, root,
                                &parameterLayout)) {
    assert(0 <= functionIndex && functionIndex < k_numberOfFunctions);
    CodePoint xnt = CodePointAtIndexInCycle(
        xntIndex, k_parameteredFunctions[functionIndex].XNTcycle, cycleSize);
    Poincare::SerializationHelper::CodePoint(buffer, bufferSize, xnt);
    if (childIndex == Parametric::FunctionIndex(static_cast<BlockType>(
                          k_parameteredFunctions[functionIndex].layoutType))) {
      if (isValidXNTParameter(parameterLayout)) {
        Serialize(parameterLayout, buffer, buffer + bufferSize);
        *cycleSize = 1;
      }
    }
    assert(strlen(buffer) > 0);
    return true;
  }
  assert(strlen(buffer) == 0);
  return false;
}

}  // namespace PoincareJ
