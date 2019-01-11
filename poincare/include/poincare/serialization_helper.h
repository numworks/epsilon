#ifndef POINCARE_SERIALIZATION_HELPER_H
#define POINCARE_SERIALIZATION_HELPER_H

#include <poincare/tree_node.h>
#include <kandinsky/include/kandinsky/unicode/code_point.h>

namespace Poincare {

namespace SerializationHelper {
  // SerializableReference to text
  int Infix(
      const TreeNode * node,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName,
      int firstChildIndex = 0,
      int lastChildIndex = -1);

  int Prefix(
      const TreeNode * node,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName,
      bool writeFirstChild = true);

  // Write one char in a buffer
  int Char(char * buffer, int bufferSize, char c);
  // Write one code point in a buffer
  constexpr int MaxSerializedCodePointSize = CodePoint::MaxCodePointCharLength + 1; // Null-terminating char
  int CodePoint(char * buffer, int bufferSize, CodePoint c);
};

}

#endif
