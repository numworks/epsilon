#include <omg/utf8_helper.h>
#include <poincare/code_points.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/parametric.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/xnt.h>

#include "k_tree.h"
#include "parsing/helper.h"
#include "parsing/tokenizer.h"
#include "rack_layout_decoder.h"
#include "serialize.h"

using namespace Poincare::Internal;

namespace Poincare::XNT {

// Cycles
constexpr int k_maxCycleSize = 5;
constexpr CodePoint k_defaultXNTCycle[] = {
    CodePoints::k_cartesianSymbol,
    CodePoints::k_sequenceSymbol,
    CodePoints::k_parametricSymbol,
    CodePoints::k_polarSymbol,
    UCodePointNull,
};
constexpr CodePoint k_defaultContinuousXNTCycle[] = {
    CodePoints::k_cartesianSymbol,
    CodePoints::k_parametricSymbol,
    CodePoints::k_polarSymbol,
    UCodePointNull,
};
constexpr CodePoint k_defaultDiscreteXNTCycle[] = {
    'k',
    'n',
    UCodePointNull,
};

static int indexOfCodePointInCycle(CodePoint codePoint,
                                   const CodePoint* cycle) {
  if (codePoint == CodePoints::k_radiusSymbol) {
    // r is not in the cycle, use θ instead
    codePoint = CodePoints::k_polarSymbol;
  }
  for (size_t i = 0; i < k_maxCycleSize - 1; i++) {
    if (cycle[i] == codePoint) {
      return i;
    }
  }
  assert(cycle[k_maxCycleSize - 1] == codePoint);
  return k_maxCycleSize - 1;
}

static size_t sizeOfCycle(const CodePoint* cycle) {
  return indexOfCodePointInCycle(UCodePointNull, cycle);
}

static CodePoint codePointAtIndexInCycle(int index, int startingIndex,
                                         const CodePoint* cycle,
                                         size_t* cycleSize) {
  assert(index >= 0);
  assert(cycleSize);
  *cycleSize = sizeOfCycle(cycle);
  assert(0 <= startingIndex && startingIndex < static_cast<int>(*cycleSize));
  return cycle[(startingIndex + index) % *cycleSize];
}

CodePoint CodePointAtIndexInDefaultCycle(int index, CodePoint startingCodePoint,
                                         size_t* cycleSize) {
  int startingIndex =
      indexOfCodePointInCycle(startingCodePoint, k_defaultXNTCycle);
  return codePointAtIndexInCycle(index, startingIndex, k_defaultXNTCycle,
                                 cycleSize);
}

CodePoint CodePointAtIndexInCycle(int index, const CodePoint* cycle,
                                  size_t* cycleSize) {
  return codePointAtIndexInCycle(index, 0, cycle, cycleSize);
}

// Parametered functions
constexpr struct {
  LayoutType layoutType;
  Type expressionType;
  const CodePoint* XNTcycle;
} k_parameteredFunctions[] = {
#if POINCARE_DIFF
    {LayoutType::Diff, Type::Diff, k_defaultContinuousXNTCycle},
#endif
#if POINCARE_INTEGRAL
    {LayoutType::Integral, Type::Integral, k_defaultContinuousXNTCycle},
#endif
#if POINCARE_SUM_AND_PRODUCT
    {LayoutType::Sum, Type::Sum, k_defaultDiscreteXNTCycle},
    {LayoutType::Product, Type::Product, k_defaultDiscreteXNTCycle},
#endif
#if POINCARE_LIST
    {LayoutType::ListSequence, Type::ListSequence, k_defaultDiscreteXNTCycle},
#endif
};
#if POINCARE_DIFF | POINCARE_INTEGRAL | POINCARE_SUM_AND_PRODUCT | POINCARE_LIST
constexpr int k_numberOfFunctions = std::size(k_parameteredFunctions);
#else
constexpr int k_numberOfFunctions = 0;
#endif

static bool Contains(UnicodeDecoder& string, UnicodeDecoder& pattern) {
  while (CodePoint c = pattern.nextCodePoint()) {
    if (string.nextCodePoint() != c) {
      return false;
    }
  }
  return true;
}

static bool findParameteredFunction1D(UnicodeDecoder& decoder,
                                      int* functionIndex, int* childIndex) {
  assert(functionIndex && childIndex);
  *functionIndex = -1;
  *childIndex = -1;
  // Step 1 : Identify the function the cursor is in
  size_t textStart = decoder.start();
  size_t location = decoder.position();
  CodePoint c = UCodePointUnknown;
  // Analyze glyphs on the left of the cursor
  if (location > textStart) {
    c = decoder.previousCodePoint();
    location = decoder.position();
  }
  int functionLevel = 0;
  int numberOfCommas = 0;
  bool functionFound = false;
  while (location > textStart && !functionFound) {
    switch (c) {
      case '(':
        // Check if we are skipping to the next matching '('.
        if (functionLevel > 0) {
          functionLevel--;
          break;
        }
        // Skip over whitespace.
        while (location > textStart && decoder.previousCodePoint() == ' ') {
          location = decoder.position();
        }
        // Move back right before the last non whitespace code-point
        decoder.nextCodePoint();
        location = decoder.position();
        // Identify one of the functions
        for (size_t i = 0; i < k_numberOfFunctions; i++) {
          const char* name = Builtin::GetReservedFunction(
                                 k_parameteredFunctions[i].expressionType)
                                 ->aliases()
                                 ->mainAlias();
          size_t length = UTF8Helper::StringCodePointLength(name);
          if (location >= textStart + length) {
            UTF8Decoder nameDecoder(name);
            size_t savePosition = decoder.position();
            // Move the decoder where the function name could start
            decoder.unsafeSetPosition(savePosition - length);
            if (Contains(decoder, nameDecoder)) {
              *functionIndex = i;
              *childIndex = numberOfCommas;
              functionFound = true;
            }
            decoder.unsafeSetPosition(savePosition);
          }
        }
        if (!functionFound) {
          // No function found, reset search parameters
          numberOfCommas = 0;
        }
        break;
      case ',':
        if (functionLevel == 0) {
          numberOfCommas++;
          if (numberOfCommas > ParsingHelper::k_indexOfParameter1D) {
            /* We are only interested in the 2 first children.
             * Look for one in level. */
            functionLevel++;
            numberOfCommas = 0;
          }
        }
        break;
      case ')':
        // Skip to the next matching '('.
        functionLevel++;
        break;
    }
    c = decoder.previousCodePoint();
    location = decoder.position();
  }
  if (functionFound) {
    // Put decoder at the beginning of the argument
    c = decoder.nextCodePoint();
    do {
      c = decoder.nextCodePoint();
    } while (c == ' ');
    assert(c == '(');
  }
  return functionFound;
}

bool FindXNTSymbol1D(UnicodeDecoder& decoder, char* buffer, size_t bufferSize,
                     int xntIndex, size_t* cycleSize) {
  assert(cycleSize);
  int functionIndex;
  int childIndex;
  buffer[0] = 0;
  *cycleSize = 0;
  if (findParameteredFunction1D(decoder, &functionIndex, &childIndex)) {
    assert(0 <= functionIndex && functionIndex < k_numberOfFunctions);
    assert(0 <= childIndex &&
           childIndex <= ParsingHelper::k_indexOfParameter1D);
    CodePoint xnt = CodePointAtIndexInCycle(
        xntIndex, k_parameteredFunctions[functionIndex].XNTcycle, cycleSize);
    size_t size = UTF8Decoder::CodePointToChars(xnt, buffer, bufferSize);
    buffer[size] = 0;
    if (childIndex == ParsingHelper::k_indexOfMainExpression1D) {
      size_t parameterStart;
      size_t parameterLength;
      if (ParsingHelper::ParameterText(decoder, &parameterStart,
                                       &parameterLength)) {
        decoder.printInBuffer(buffer, bufferSize, parameterLength);
        assert(buffer[parameterLength] == 0);
        *cycleSize = 1;
      }
    }
    assert(strlen(buffer) > 0);
    return true;
  }
  assert(strlen(buffer) == 0);
  return false;
}

constexpr int k_indexOfParameter = Parametric::k_variableIndex;

static bool findParameteredFunction2D(const Tree* layout, const Tree* root,
                                      int* functionIndex, int* childIndex,
                                      const Tree** parameterLayout) {
  assert(functionIndex && childIndex && parameterLayout);
  *functionIndex = -1;
  *childIndex = -1;
  *parameterLayout = nullptr;
  assert(layout);
  const Tree* child = layout;
  const Tree* parent = root->parentOfDescendant(child, childIndex);
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

static bool isValidXNTParameter(const Tree* xnt) {
  if (xnt->hasChildSatisfying(
          [](const Tree* e) { return !e->isCodePointLayout(); })) {
    return false;
  }
  RackLayoutDecoder decoder(xnt);
  return Tokenizer::CanBeCustomIdentifier(decoder);
}

bool FindXNTSymbol2D(const Tree* layout, const Tree* root, char* buffer,
                     size_t bufferSize, int xntIndex, size_t* cycleSize) {
  assert(cycleSize);
  int functionIndex;
  int childIndex;
  const Tree* parameterLayout;
  buffer[0] = 0;
  *cycleSize = 0;
  if (findParameteredFunction2D(layout, root, &functionIndex, &childIndex,
                                &parameterLayout)) {
    assert(0 <= functionIndex && functionIndex < k_numberOfFunctions);
    CodePoint xnt = CodePointAtIndexInCycle(
        xntIndex, k_parameteredFunctions[functionIndex].XNTcycle, cycleSize);
    size_t size = UTF8Decoder::CodePointToChars(xnt, buffer, bufferSize);
    buffer[size] = 0;
    if (childIndex == Parametric::FunctionIndex(static_cast<Type>(
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

}  // namespace Poincare::XNT
