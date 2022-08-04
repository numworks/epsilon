#include <poincare/serialization_helper.h>
#include <poincare/expression_node.h>
#include <poincare/rational.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

void SerializationHelper::ReplaceSystemParenthesesByUserParentheses(char * buffer, int length) {
  assert(UTF8Decoder::CharSizeOfCodePoint(UCodePointLeftSystemParenthesis == 1));
  assert(UTF8Decoder::CharSizeOfCodePoint(UCodePointRightSystemParenthesis == 1));
  assert(UTF8Decoder::CharSizeOfCodePoint('(' == 1));
  assert(UTF8Decoder::CharSizeOfCodePoint(')' == 1));
  assert(length == -1 || length > 0);

  int offset = 0;
  char c = *(buffer + offset);
  while (c != 0) {
    if (c == UCodePointLeftSystemParenthesis) {
      *(buffer + offset) = '(';
    } else if (c == UCodePointRightSystemParenthesis) {
      *(buffer + offset) = ')';
    }
    offset++;
    if (length >= 0 && offset > length - 1) {
      break;
    }
    c = *(buffer + offset);
  }
}

static bool checkBufferSize(char * buffer, int bufferSize, int * result) {
  // If buffer has size 0 or 1, put a zero if it fits and return
  if (bufferSize == 0) {
    *result = -1;
    return true;
  }

  buffer[bufferSize-1] = 0; // Null-terminate the buffer
  if (bufferSize == 1) {
    *result = 0;
    return true;
  }
  return false;
}

int SerializationHelper::SerializeChild(
    const TreeNode * childNode,
    const TreeNode * parentNode,
    char * buffer,
    int bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits)
{
  {
    int result = 0;
    if (checkBufferSize(buffer, bufferSize, &result)) {
      return result;
    }
  }

  int numberOfChar = 0;
  // Write the child with parentheses if needed
  bool addParentheses = parentNode->childNeedsSystemParenthesesAtSerialization(childNode);
  if (addParentheses) {
    numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize - numberOfChar, UCodePointLeftSystemParenthesis);
    if (numberOfChar >= bufferSize-1) {
      return bufferSize-1;
    }
  }
  numberOfChar+= childNode->serialize(buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode, numberOfDigits);
  if (numberOfChar >= bufferSize-1) {
    assert(buffer[bufferSize - 1] == 0);
    return bufferSize-1;
  }
  if (addParentheses) {
    numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize - numberOfChar, UCodePointRightSystemParenthesis);
    if (numberOfChar >= bufferSize-1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize-1;
    }
  }
  assert(buffer[numberOfChar] == 0);
  return numberOfChar;
}

int InfixPrefix(
    bool prefix,
    const TreeNode * node,
    char * buffer,
    int bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits,
    const char * operatorName,
    SerializationHelper::TypeOfParenthesis typeOfParenthesis,
    int firstChildIndex,
    int lastChildIndex)
{
  {
    int result = 0;
    if (checkBufferSize(buffer, bufferSize, &result)) {
      return result;
    }
  }

  int numberOfChar = 0;
  CodePoint openingCodePoint = UCodePointNull;
  CodePoint closingCodePoint = UCodePointNull;

  if (prefix) {
    // Prefix: Copy the operator name
    numberOfChar = strlcpy(buffer, operatorName, bufferSize);
    if (numberOfChar >= bufferSize-1) {
      // Erase the inserted chars to prevent truncated operator names.
      memset(buffer, 0, bufferSize);
      return bufferSize-1;
    }
    // Add the opening (system or user) parenthesis
    switch (typeOfParenthesis) {
      case SerializationHelper::TypeOfParenthesis::Classic:
        openingCodePoint = CodePoint('(');
        closingCodePoint = CodePoint(')');
        break;
      case SerializationHelper::TypeOfParenthesis::Braces:
        openingCodePoint = CodePoint('{');
        closingCodePoint = CodePoint('}');
        break;
      case SerializationHelper::TypeOfParenthesis::System:
        openingCodePoint = UCodePointLeftSystemParenthesis;
        closingCodePoint = UCodePointRightSystemParenthesis;
        break;
      default:
        assert(typeOfParenthesis == SerializationHelper::TypeOfParenthesis::None);
    }
    if (typeOfParenthesis != SerializationHelper::TypeOfParenthesis::None) {
      numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize - numberOfChar, openingCodePoint);
    }
    if (numberOfChar >= bufferSize-1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize - 1;
    }
  }

  int childrenCount = node->numberOfChildren();
  assert(prefix || childrenCount > 0);

  if (childrenCount > 0) {
    int lastIndex = lastChildIndex < 0 ? childrenCount - 1 : lastChildIndex;
    assert(firstChildIndex <= lastIndex);

    // Write the children, separated with commas or the operator
    for (int i = firstChildIndex; i <= lastIndex; i++) {
      if (i != firstChildIndex) {
        // Write the operator or the comma
        numberOfChar += prefix ?
          SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize - numberOfChar, ',') :
          strlcpy(buffer+numberOfChar, operatorName, bufferSize-numberOfChar);
        if (numberOfChar >= bufferSize-1) {
          assert(buffer[bufferSize - 1] == 0);
          return bufferSize - 1;
        }
      }
      // Write the child, with or without parentheses if needed
      if (prefix) {
        if (typeOfParenthesis == SerializationHelper::TypeOfParenthesis::System && (childrenCount > 1)) {
          numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize - numberOfChar, UCodePointLeftSystemParenthesis);
          if (numberOfChar >= bufferSize-1) {
            assert(buffer[bufferSize - 1] == 0);
            return bufferSize - 1;
          }
        }
        numberOfChar += node->childAtIndex(i)->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfDigits);
        if (numberOfChar >= bufferSize-1) {
          assert(buffer[bufferSize - 1] == 0);
          return bufferSize - 1;
        }
        if (typeOfParenthesis == SerializationHelper::TypeOfParenthesis::System && (childrenCount > 1)) {
          numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize - numberOfChar, UCodePointRightSystemParenthesis);
          if (numberOfChar >= bufferSize-1) {
            assert(buffer[bufferSize - 1] == 0);
            return bufferSize - 1;
          }
        }
      } else {
        numberOfChar += SerializationHelper::SerializeChild(node->childAtIndex(i), node, buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode, numberOfDigits);
        if (numberOfChar >= bufferSize-1) {
          assert(buffer[bufferSize - 1] == 0);
          return bufferSize - 1;
        }
      }
    }
  }

  if (prefix && typeOfParenthesis != SerializationHelper::TypeOfParenthesis::None) {
    // Add the closing parenthesis
    numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize - numberOfChar, closingCodePoint);
    if (numberOfChar >= bufferSize-1) {
       assert(buffer[bufferSize - 1] == 0);
       return bufferSize - 1;
    }
  }

  assert(buffer[numberOfChar] == 0);
  return numberOfChar;
}

int SerializationHelper::Infix(
    const TreeNode * node,
    char * buffer,
    int bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits,
    const char * operatorName,
    int firstChildIndex,
    int lastChildIndex)
{
  return InfixPrefix(false, node, buffer, bufferSize, floatDisplayMode, numberOfDigits, operatorName, TypeOfParenthesis::None, firstChildIndex, lastChildIndex);
}

int SerializationHelper::Prefix(
    const TreeNode * node,
    char * buffer,
    int bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits,
    const char * operatorName,
    TypeOfParenthesis typeOfParenthesis,
    int lastChildIndex)
{
  return InfixPrefix(true, node, buffer, bufferSize, floatDisplayMode, numberOfDigits, operatorName, typeOfParenthesis, 0, lastChildIndex);
}

int SerializationHelper::CodePoint(char * buffer, int bufferSize, class CodePoint c) {
  {
    int result = 0;
    if (checkBufferSize(buffer, bufferSize, &result)) {
      return result;
    }
  }
  size_t length = UTF8Decoder::CharSizeOfCodePoint(c);
  if (static_cast<int>(length) >= bufferSize) {
    /* Code point doesn't fit, nullify the rest of the buffer to prevent
     * truncated utf8 characters */
    memset(buffer, 0, bufferSize);
  } else {
    UTF8Decoder::CodePointToChars(c, buffer, bufferSize - 1);
    buffer[length] = 0;
  }
  return length;
}

bool SerializationHelper::PostfixChildNeedsSystemParenthesesAtSerialization(const TreeNode * child) {
  if (static_cast<const ExpressionNode *>(child)->type() == ExpressionNode::Type::Rational && !static_cast<const RationalNode *>(child)->isInteger()) {
    return true;
  }
  return static_cast<const ExpressionNode *>(child)->isOfType({ExpressionNode::Type::Division, ExpressionNode::Type::Power});
}

}
