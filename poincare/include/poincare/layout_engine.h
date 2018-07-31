#ifndef POINCARE_LAYOUT_ENGINE_H
#define POINCARE_LAYOUT_ENGINE_H

#include <poincare/expression.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class LayoutEngine {

public:
  /* Expression to Layout */
  static LayoutRef createInfixLayout(const Expression * expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);
  static LayoutRef createPrefixLayout(const Expression * expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);

  /* Create special layouts */
  static LayoutRef createParenthesedLayout(LayoutRef layout, bool cloneLayout);
  static LayoutRef createStringLayout(const char * buffer, int bufferSize, KDText::FontSize fontSize = KDText::FontSize::Large);
  static LayoutRef createLogLayout(LayoutRef argument, LayoutRef index);

  /* Expression to Text */
  static int writeInfixExpressionTextInBuffer(
      const Expression * expression,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName);
  static int writePrefixExpressionTextInBuffer(
      const Expression * expression,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName);

  /* SerializableReference to Text */
  static int writeInfixSerializableRefTextInBuffer(
      const SerializableRef serializableRef,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName,
      int firstChildIndex = 0,
      int lastChildIndex = -1);

  static int writePrefixSerializableRefTextInBuffer(
      const SerializableRef serializableRef,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName,
      bool writeFirstChild = true);

  /* Write one char in buffer */
  static int writeOneCharInBuffer(char * buffer, int bufferSize, char charToWrite);

private:
  static constexpr char divideChar = '/';
  // These two functions return the index of the null-terminating char.
  static int writeInfixExpressionOrExpressionLayoutTextInBuffer(const Expression * expression, void * expressionLayout, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfDigits, const char * operatorName, int firstChildIndex, int lastChildIndex);
  static int writePrefixExpressionOrExpressionLayoutTextInBuffer(const Expression * expression, void * expressionLayout, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfDigits, const char * operatorName, bool writeFirstChild = true);

  static void writeChildTreeInBuffer(SerializableRef childRef, SerializableRef parentRef, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfDigits, int * numberOfChar);
};

}

#endif
