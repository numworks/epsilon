#include <assert.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/expression_node.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <string.h>

namespace Poincare {

size_t SerializationHelper::ReplaceSystemParenthesesAndBracesByUserParentheses(
    char *buffer, size_t length) {
  assert(
      UTF8Decoder::CharSizeOfCodePoint(UCodePointLeftSystemParenthesis == 1));
  assert(
      UTF8Decoder::CharSizeOfCodePoint(UCodePointRightSystemParenthesis == 1));
  assert(UTF8Decoder::CharSizeOfCodePoint('(' == 1));
  assert(UTF8Decoder::CharSizeOfCodePoint(')' == 1));
  assert(UTF8Decoder::CharSizeOfCodePoint(UCodePointSystem == 1));
  assert(UTF8Decoder::CharSizeOfCodePoint('{' == 1));
  assert(UTF8Decoder::CharSizeOfCodePoint('}' == 1));

  if (length < 0) {
    length = strlen(buffer);
  }

  size_t offset = 0;
  char c = *(buffer + offset);
  bool pendingSystemCodePoint = false;
  while (c != 0) {
    if (pendingSystemCodePoint && (c == '{' || c == '}')) {
      *(buffer + offset) = c == '{' ? '(' : ')';
      strlcpy(buffer + offset - 1, buffer + offset,
              strlen(buffer + offset) + 1);
      length--;
      offset--;
    } else if (c == UCodePointLeftSystemParenthesis) {
      *(buffer + offset) = '(';
    } else if (c == UCodePointRightSystemParenthesis) {
      *(buffer + offset) = ')';
    }
    offset++;
    if (length >= 0 && offset > length - 1) {
      break;
    }
    pendingSystemCodePoint = c == UCodePointSystem;
    c = *(buffer + offset);
  }
  return length;
}

static bool checkBufferSize(char *buffer, size_t bufferSize, size_t *result) {
  // If buffer has size 0 or 1, put a zero if it fits and return
  if (bufferSize == 0) {
    *result = -1;
    return true;
  }

  buffer[bufferSize - 1] = 0;  // Null-terminate the buffer
  if (bufferSize == 1) {
    *result = 0;
    return true;
  }
  return false;
}

size_t SerializationHelper::SerializeChild(
    const PoolObject *childNode, const PoolObject *parentNode, char *buffer,
    size_t bufferSize, Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits) {
  {
    size_t result = 0;
    if (checkBufferSize(buffer, bufferSize, &result)) {
      return result;
    }
  }

  size_t numberOfChar = 0;
  // Write the child with parentheses if needed
  bool addParentheses =
      parentNode->childNeedsSystemParenthesesAtSerialization(childNode);
  if (addParentheses) {
    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar,
        UCodePointLeftSystemParenthesis);
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
  }
  numberOfChar +=
      childNode->serialize(buffer + numberOfChar, bufferSize - numberOfChar,
                           floatDisplayMode, numberOfDigits);
  if (numberOfChar >= bufferSize - 1) {
    assert(buffer[bufferSize - 1] == 0);
    return bufferSize - 1;
  }
  if (addParentheses) {
    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar,
        UCodePointRightSystemParenthesis);
    if (numberOfChar >= bufferSize - 1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize - 1;
    }
  }
  assert(buffer[numberOfChar] == 0);
  return numberOfChar;
}

size_t InfixPrefix(bool prefix, const PoolObject *node, char *buffer,
                   size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfDigits, const char *operatorName,
                   SerializationHelper::ParenthesisType typeOfParenthesis,
                   int firstChildIndex, int lastChildIndex) {
  {
    size_t result = 0;
    if (checkBufferSize(buffer, bufferSize, &result)) {
      return result;
    }
  }

  size_t numberOfChar = 0;
  CodePoint openingCodePoint = UCodePointNull;
  CodePoint closingCodePoint = UCodePointNull;

  size_t operatorNameLength = strlen(operatorName);

  if (prefix) {
    // Prefix: Copy the operator name
    if (numberOfChar + operatorNameLength >= bufferSize - 1) {
      buffer[numberOfChar] = 0;
      return numberOfChar;
    }
    numberOfChar = strlcpy(buffer, operatorName, bufferSize);
    assert(numberOfChar < bufferSize - 1);
    // Add the opening (system or user) parenthesis
    switch (typeOfParenthesis) {
      case SerializationHelper::ParenthesisType::Classic:
        openingCodePoint = CodePoint('(');
        closingCodePoint = CodePoint(')');
        break;
      case SerializationHelper::ParenthesisType::Braces:
        openingCodePoint = CodePoint('{');
        closingCodePoint = CodePoint('}');
        break;
      case SerializationHelper::ParenthesisType::System:
        openingCodePoint = UCodePointLeftSystemParenthesis;
        closingCodePoint = UCodePointRightSystemParenthesis;
        break;
      default:
        assert(typeOfParenthesis == SerializationHelper::ParenthesisType::None);
    }
    if (typeOfParenthesis != SerializationHelper::ParenthesisType::None) {
      if (typeOfParenthesis == SerializationHelper::ParenthesisType::Braces) {
        numberOfChar += SerializationHelper::CodePoint(
            buffer + numberOfChar, bufferSize - numberOfChar, UCodePointSystem);
      }
      numberOfChar += SerializationHelper::CodePoint(
          buffer + numberOfChar, bufferSize - numberOfChar, openingCodePoint);
    }
    if (numberOfChar >= bufferSize - 1) {
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
        if (prefix) {
          numberOfChar += SerializationHelper::CodePoint(
              buffer + numberOfChar, bufferSize - numberOfChar, ',');
          if (numberOfChar >= bufferSize - 1) {
            assert(buffer[bufferSize - 1] == 0);
            return bufferSize - 1;
          }
        } else {
          if (numberOfChar + operatorNameLength >= bufferSize - 1) {
            assert(buffer[numberOfChar] == 0);
            return numberOfChar;
          }
          numberOfChar += strlcpy(buffer + numberOfChar, operatorName,
                                  bufferSize - numberOfChar);
          assert(numberOfChar < bufferSize - 1);
        }
      }
      // Write the child, with or without parentheses if needed
      if (prefix) {
        if (typeOfParenthesis == SerializationHelper::ParenthesisType::System &&
            (childrenCount > 1)) {
          numberOfChar += SerializationHelper::CodePoint(
              buffer + numberOfChar, bufferSize - numberOfChar,
              UCodePointLeftSystemParenthesis);
          if (numberOfChar >= bufferSize - 1) {
            assert(buffer[bufferSize - 1] == 0);
            return bufferSize - 1;
          }
        }
        numberOfChar += node->childAtIndex(i)->serialize(
            buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode,
            numberOfDigits);
        if (numberOfChar >= bufferSize - 1) {
          assert(buffer[bufferSize - 1] == 0);
          return bufferSize - 1;
        }
        if (typeOfParenthesis == SerializationHelper::ParenthesisType::System &&
            (childrenCount > 1)) {
          numberOfChar += SerializationHelper::CodePoint(
              buffer + numberOfChar, bufferSize - numberOfChar,
              UCodePointRightSystemParenthesis);
          if (numberOfChar >= bufferSize - 1) {
            assert(buffer[bufferSize - 1] == 0);
            return bufferSize - 1;
          }
        }
      } else {
        numberOfChar += SerializationHelper::SerializeChild(
            node->childAtIndex(i), node, buffer + numberOfChar,
            bufferSize - numberOfChar, floatDisplayMode, numberOfDigits);
        if (numberOfChar >= bufferSize - 1) {
          assert(buffer[bufferSize - 1] == 0);
          return bufferSize - 1;
        }
      }
    }
  }

  if (prefix &&
      typeOfParenthesis != SerializationHelper::ParenthesisType::None) {
    // Add the closing parenthesis
    if (typeOfParenthesis == SerializationHelper::ParenthesisType::Braces) {
      numberOfChar += SerializationHelper::CodePoint(
          buffer + numberOfChar, bufferSize - numberOfChar, UCodePointSystem);
    }
    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar, closingCodePoint);
    if (numberOfChar >= bufferSize - 1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize - 1;
    }
  }

  assert(buffer[numberOfChar] == 0);
  return numberOfChar;
}

size_t SerializationHelper::Infix(const PoolObject *node, char *buffer,
                                  size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfDigits, const char *operatorName,
                                  int firstChildIndex, int lastChildIndex) {
  return InfixPrefix(false, node, buffer, bufferSize, floatDisplayMode,
                     numberOfDigits, operatorName, ParenthesisType::None,
                     firstChildIndex, lastChildIndex);
}

size_t SerializationHelper::Prefix(const PoolObject *node, char *buffer,
                                   size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfDigits, const char *operatorName,
                                   ParenthesisType typeOfParenthesis,
                                   int lastChildIndex) {
  return InfixPrefix(true, node, buffer, bufferSize, floatDisplayMode,
                     numberOfDigits, operatorName, typeOfParenthesis, 0,
                     lastChildIndex);
}

size_t SerializationHelper::CodePoint(char *buffer, size_t bufferSize,
                                      class CodePoint c) {
  {
    size_t result = 0;
    if (checkBufferSize(buffer, bufferSize, &result)) {
      return result;
    }
  }
  size_t length = UTF8Decoder::CharSizeOfCodePoint(c);
  if (length >= bufferSize) {
    /* Code point doesn't fit, nullify the rest of the buffer to prevent
     * truncated utf8 characters */
    memset(buffer, 0, bufferSize);
  } else {
    UTF8Decoder::CodePointToChars(c, buffer, bufferSize - 1);
    buffer[length] = 0;
  }
  return length;
}

bool SerializationHelper::PostfixChildNeedsSystemParenthesesAtSerialization(
    const PoolObject *child) {
  if (static_cast<const ExpressionNode *>(child)->otype() ==
          ExpressionNode::Type::Rational &&
      !static_cast<const RationalNode *>(child)->isInteger()) {
    return true;
  }
  return static_cast<const ExpressionNode *>(child)->isOfType(
      {ExpressionNode::Type::Division, ExpressionNode::Type::Power});
}

}  // namespace Poincare
