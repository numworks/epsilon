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

  void ReplaceSystemParenthesesByUserParentheses(char * buffer, int length = -1);

  // SerializableReference to text
  int Infix(
      const TreeNode * node,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName,
      int firstChildIndex = 0,
      int lastChildIndex = -1,
      bool bracesInsteadOfParentheses = false);

  /* needsSystemParentheses add System parentheses wrapping the layout children.
   * It is used when serializing layouts to avoid creating a parsable string
   * from a misformed layout. For instance, we don't want to parse:
   * |2)(3|, so we serialize it in "abs({2)(3})" where {} are System parentheses
   * instead of "abs(2)(3)".
   *
   * /!\ A layout that calls Prefix should put true to needsSystemParentheses */
  int Prefix(
      const TreeNode * node,
      char * buffer,
      int bufferSize,
      Preferences::PrintFloatMode floatDisplayMode,
      int numberOfDigits,
      const char * operatorName,
      bool needsSystemParentheses = false,
      int lastChildIndex = -1,
      bool bracesInsteadOfParentheses = false);

  int SerializeChild(
    const TreeNode * childNode,
    const TreeNode * parentNode,
    char * buffer,
    int bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits);

  // Write one code point in a buffer and a null-terminating char
  int CodePoint(char * buffer, int bufferSize, CodePoint c);

  /* Default childNeedsSystemParenthesesAtSerialization for postfix operators
   * such as % and ! */
  bool PostfixChildNeedsSystemParenthesesAtSerialization(const TreeNode * child);
};

}

#endif
