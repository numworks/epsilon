#include <poincare/linear_layout_decoder.h>
#include <poincare/xnt_helpers.h>
#include <poincare_expressions.h>

#include <array>

#include "parsing/tokenizer.h"

namespace Poincare {

namespace XNTHelpers {
static bool findParameteredFunction1D(UnicodeDecoder& decoder,
                                      const char* const* functionsAlias,
                                      int numberOfFuctions, int maxChildIndex,
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
        for (size_t i = 0; i < numberOfFuctions; i++) {
          const char* name = functionsAlias[i];
          size_t length = strlen(name);
          if (location >= textStart + length) {
            size_t savePosition = decoder.position();
            // Move the decoder where the function name could start
            decoder.unsafeSetPosition(savePosition - length);
            constexpr size_t bufferSize = 10;
            char buffer[bufferSize];
            decoder.printInBuffer(buffer, bufferSize, length);
            if (strcmp(buffer, name) == 0) {
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
          if (numberOfCommas > maxChildIndex) {
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

bool FindXNTSymbol1D(UnicodeDecoder& decoder, char* buffer, size_t bufferSize) {
  constexpr int k_numberOfFunctions = 5;
  constexpr static const char* k_functionsAlias[k_numberOfFunctions] = {
      Derivative::s_functionHelper.aliasesList().mainAlias(),
      Integral::s_functionHelper.aliasesList().mainAlias(),
      Product::s_functionHelper.aliasesList().mainAlias(),
      Sum::s_functionHelper.aliasesList().mainAlias(),
      ListSequence::s_functionHelper.aliasesList().mainAlias(),
  };
  constexpr static const char k_functionsXNT[k_numberOfFunctions] = {
      Derivative::k_defaultXNTChar,   Integral::k_defaultXNTChar,
      Product::k_defaultXNTChar,      Sum::k_defaultXNTChar,
      ListSequence::k_defaultXNTChar,
  };
  constexpr static const int k_indexOfMainExpression = 0;
  constexpr static const int k_indexOfParameter = 1;
  int functionIndex;
  int childIndex;
  buffer[0] = 0;
  if (findParameteredFunction1D(decoder, k_functionsAlias, k_numberOfFunctions,
                                k_indexOfParameter, &functionIndex,
                                &childIndex)) {
    assert(0 <= functionIndex && functionIndex < k_numberOfFunctions);
    assert(0 <= childIndex && childIndex <= k_indexOfParameter);
    SerializationHelper::CodePoint(buffer, bufferSize,
                                   k_functionsXNT[functionIndex]);
    if (childIndex == k_indexOfMainExpression) {
      size_t parameterStart;
      size_t parameterLength;
      if (ParameteredExpression::ParameterText(decoder, &parameterStart,
                                               &parameterLength) &&
          bufferSize - 1 >= parameterLength) {
        decoder.printInBuffer(buffer, bufferSize, parameterLength);
        assert(buffer[parameterLength] == 0);
      }
    }
    return true;
  }
  return false;
}

static Layout xntLayout(Layout parameterLayout) {
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
}

bool FindXNTSymbol2D(Layout layout, char* buffer, size_t bufferSize) {
  constexpr int k_numberOfFunctions = 6;
  constexpr static LayoutNode::Type k_functionsType[k_numberOfFunctions] = {
      LayoutNode::Type::FirstOrderDerivativeLayout,
      LayoutNode::Type::HigherOrderDerivativeLayout,
      LayoutNode::Type::IntegralLayout,
      LayoutNode::Type::ProductLayout,
      LayoutNode::Type::SumLayout,
      LayoutNode::Type::ListSequenceLayout,
  };
  constexpr static const char k_functionsXNT[k_numberOfFunctions] = {
      Derivative::k_defaultXNTChar, Derivative::k_defaultXNTChar,
      Integral::k_defaultXNTChar,   Product::k_defaultXNTChar,
      Sum::k_defaultXNTChar,        ListSequence::k_defaultXNTChar,
  };
  constexpr static const int k_indexOfMainExpression = 0;
  constexpr static const int k_indexOfParameter = 1;
  int functionIndex = -1;
  int childIndex = -1;
  Layout parameterLayout;
  assert(!layout.isUninitialized());
  Layout child = layout;
  Layout parent = child.parent();
  bool functionFound = false;
  while (!parent.isUninitialized() && !functionFound) {
    childIndex = parent.indexOfChild(child);
    if (childIndex <= k_indexOfParameter) {
      for (size_t i = 0; i < k_numberOfFunctions; i++) {
        if (parent.type() == k_functionsType[i]) {
          functionIndex = i;
          parameterLayout = parent.childAtIndex(k_indexOfParameter);
          functionFound = true;
          break;
        }
      }
    }
    child = parent;
    parent = child.parent();
  }
  buffer[0] = 0;
  if (functionFound) {
    assert(0 <= functionIndex && functionIndex < k_numberOfFunctions);
    assert(0 <= childIndex && childIndex <= k_indexOfParameter);
    SerializationHelper::CodePoint(buffer, bufferSize,
                                   k_functionsXNT[functionIndex]);
    if (childIndex == k_indexOfMainExpression) {
      parameterLayout = xntLayout(parameterLayout);
      if (!parameterLayout.isUninitialized()) {
        parameterLayout.serializeForParsing(buffer, bufferSize);
      }
    }
    return true;
  }
  return false;
}

}  // namespace XNTHelpers

}  // namespace Poincare
