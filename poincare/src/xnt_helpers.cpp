#include <poincare/linear_layout_decoder.h>
#include <poincare/xnt_helpers.h>

#include <array>

#include "parsing/tokenizer.h"

namespace Poincare {

#define Layout OLayout

namespace XNTHelpers {

static int indexOfCodePointInCycle(CodePoint codePoint,
                                   const CodePoint* cycle) {
  if (codePoint == Symbol::k_radiusSymbol) {
    // r is not in the cycle, use θ instead
    codePoint = Symbol::k_polarSymbol;
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
          const char* name = k_parameteredFunctions[i].aliasesList.mainAlias();
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
          if (numberOfCommas > k_indexOfParameter) {
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
    assert(0 <= childIndex && childIndex <= k_indexOfParameter);
    CodePoint xnt = CodePointAtIndexInCycle(
        xntIndex, k_parameteredFunctions[functionIndex].XNTcycle, cycleSize);
    SerializationHelper::CodePoint(buffer, bufferSize, xnt);
    if (childIndex == k_indexOfMainExpression) {
      size_t parameterStart;
      size_t parameterLength;
      if (ParameteredExpression::ParameterText(decoder, &parameterStart,
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

static Layout xntLayout(Layout parameterLayout) {
#if 0
  if (parameterLayout.isUninitialized()) {
    return Layout();
  }
  Layout xnt = parameterLayout.clone();
  if (!xnt.isCodePointsString() &&
      xnt.type() != LayoutNode::Type::CodePointLayout &&
      xnt.type() != LayoutNode::Type::CombinedCodePointsLayout) {
    return Layout();
  }
  if (!xnt.isHorizontal()) {
    xnt = HorizontalLayout::Builder(xnt.clone());
  }
  LinearLayoutDecoder decoder(static_cast<HorizontalLayout&>(xnt));
  if (!Tokenizer::CanBeCustomIdentifier(decoder)) {
    return Layout();
  }
  return xnt;
#else
  return Layout();
#endif
}

static bool findParameteredFunction2D(Layout layout, int* functionIndex,
                                      int* childIndex,
                                      Layout* parameterLayout) {
  assert(functionIndex && childIndex && parameterLayout);
  *functionIndex = -1;
  *childIndex = -1;
  *parameterLayout = Layout();
  assert(!layout.isUninitialized());
  Layout child = layout;
  Layout parent = child.parent();
  while (!parent.isUninitialized()) {
    *childIndex = parent.indexOfChild(child);
    if (*childIndex <= k_indexOfParameter) {
      for (size_t i = 0; i < k_numberOfFunctions; i++) {
        if (parent.type() == k_parameteredFunctions[i].layoutType) {
          *functionIndex = i;
          *parameterLayout = parent.childAtIndex(k_indexOfParameter);
          return true;
        }
      }
    }
    child = parent;
    parent = child.parent();
  }
  return false;
}

bool FindXNTSymbol2D(Layout layout, char* buffer, size_t bufferSize,
                     int xntIndex, size_t* cycleSize) {
  assert(cycleSize);
  int functionIndex;
  int childIndex;
  Layout parameterLayout;
  buffer[0] = 0;
  *cycleSize = 0;
  if (findParameteredFunction2D(layout, &functionIndex, &childIndex,
                                &parameterLayout)) {
    assert(0 <= functionIndex && functionIndex < k_numberOfFunctions);
    assert(0 <= childIndex && childIndex <= k_indexOfParameter);
    CodePoint xnt = CodePointAtIndexInCycle(
        xntIndex, k_parameteredFunctions[functionIndex].XNTcycle, cycleSize);
    SerializationHelper::CodePoint(buffer, bufferSize, xnt);
    if (childIndex == k_indexOfMainExpression) {
      parameterLayout = xntLayout(parameterLayout);
      if (!parameterLayout.isUninitialized()) {
        parameterLayout.serializeForParsing(buffer, bufferSize);
        *cycleSize = 1;
      }
    }
    assert(strlen(buffer) > 0);
    return true;
  }
  assert(strlen(buffer) == 0);
  return false;
}

}  // namespace XNTHelpers

}  // namespace Poincare
