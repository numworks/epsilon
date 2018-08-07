#ifndef POINCARE_LAYOUT_ENGINE_H
#define POINCARE_LAYOUT_ENGINE_H

#include <poincare/expression.h>
#include <poincare/layout_reference.h>
#include <poincare/horizontal_layout_node.h>

namespace Poincare {

class LayoutEngine {

public:
  /* Expression to Layout */
  static LayoutRef createInfixLayout(const Expression expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);
  static LayoutRef createPrefixLayout(const Expression expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);

  /* Create special layouts */
  static LayoutRef createParenthesedLayout(LayoutRef layout, bool cloneLayout);
  static HorizontalLayoutRef createStringLayout(const char * buffer, int bufferSize, KDText::FontSize fontSize = KDText::FontSize::Large);
  static LayoutRef createLogLayout(LayoutRef argument, LayoutRef index);

  /* SerializableReference to Text */
  static int writeInfixSerializableRefTextInBuffer(
      const SerializableReference serializableRef,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName,
      int firstChildIndex = 0,
      int lastChildIndex = -1);

  static int writePrefixSerializableRefTextInBuffer(
      const SerializableReference serializableRef,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName,
      bool writeFirstChild = true);

  /* Write one char in buffer */
  static int writeOneCharInBuffer(char * buffer, int bufferSize, char charToWrite);

private:
  static void writeChildTreeInBuffer(SerializableReference childRef, SerializableReference parentRef, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfDigits, int * numberOfChar);
};

}

#endif
