#ifndef POINCARE_SERIALIZATION_HELPER_H
#define POINCARE_SERIALIZATION_HELPER_H

#include <omg/code_point.h>

#include "pool_object.h"

namespace Poincare {

/* The serialization methods write their argument as a string in the given
 * buffer, with a null-terminating 0. The return value is:
 *   -> -1 if the buffer size is 0
 *   -> Otherwise, the number of chars written, without the null terminating 0
 */

namespace SerializationHelper {

enum class ParenthesisType : uint8_t {
  Classic = 0,
  Braces,
  System,
  None,
};

// SerializableReference to text
size_t Infix(const PoolObject* node, char* buffer, size_t bufferSize,
             Preferences::PrintFloatMode floatDisplayMode, int numberOfDigits,
             const char* operatorName, int firstChildIndex = 0,
             int lastChildIndex = -1);

/* ParenthesisType::System add System parentheses wrapping the layout children.
 * It is used when serializing layouts to avoid creating a parsable string
 * from a misformed layout. For instance, we don't want to parse:
 * |2)(3|, so we serialize it in "abs({2)(3})" where {} are System parentheses
 * instead of "abs(2)(3)".
 *
 * /!\ A layout that calls Prefix should put true to needsSystemParentheses */
size_t Prefix(const PoolObject* node, char* buffer, size_t bufferSize,
              Preferences::PrintFloatMode floatDisplayMode, int numberOfDigits,
              const char* operatorName,
              ParenthesisType typeOfParenthesis = ParenthesisType::Classic,
              int lastChildIndex = -1);

size_t SerializeChild(const PoolObject* childNode, const PoolObject* parentNode,
                      char* buffer, size_t bufferSize,
                      Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfDigits);

// Write one code point in a buffer and a null-terminating char
size_t CodePoint(char* buffer, size_t bufferSize, CodePoint c);

/* Default childNeedsSystemParenthesesAtSerialization for postfix operators
 * such as % and ! */
bool PostfixChildNeedsSystemParenthesesAtSerialization(const PoolObject* child);
};  // namespace SerializationHelper

}  // namespace Poincare

#endif
