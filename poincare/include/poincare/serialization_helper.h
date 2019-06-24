#ifndef POINCARE_SERIALIZATION_HELPER_H
#define POINCARE_SERIALIZATION_HELPER_H

#include <poincare/tree_node.h>
#include <ion/unicode/code_point.h>

namespace Poincare {

/* The serialization methods write their argument as a string in the given
 * buffer, with a null-terminating 0. The return value is:
 *   -> -1 if the buffer size is 0
 *   -> Otherwise, the number of chars written, without the null terminating 0
 */

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
      int firstChildIndex = 0,
      int lastChildIndex = -1);

  // Write one code point in a buffer and a null-terminating char
  int CodePoint(char * buffer, int bufferSize, CodePoint c);
};

}

#endif
