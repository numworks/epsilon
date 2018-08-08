#ifndef POINCARE_SERIALIZATION_HELPER_H
#define POINCARE_SERIALIZATION_HELPER_H

#include <poincare/expression.h>
#include <poincare/horizontal_layout_node.h>

namespace Poincare {

namespace SerializationHelper {
  /* SerializableReference to Text */
  static int Infix(
      const SerializableReference serializableRef,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName,
      int firstChildIndex = 0,
      int lastChildIndex = -1);

  static int Prefix(
      const SerializableReference serializableRef,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName,
      bool writeFirstChild = true);

  /* Write one char in buffer */
  static int Char(char * buffer, int bufferSize, char charToWrite);
};

}

#endif
