#include <assert.h>
#include <omg/utf8_decoder.h>
#include <poincare/layout.h>
#include <poincare/old/serialization_helper.h>
#include <string.h>

#include <algorithm>

namespace Poincare {

size_t VerticalOffsetLayoutNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  if (verticalPosition() == VerticalPosition::Subscript ||
      horizontalPosition() == HorizontalPosition::Prefix) {
    if (bufferSize == 0) {
      return bufferSize - 1;
    }
    buffer[bufferSize - 1] = 0;
    if (bufferSize == 1) {
      return bufferSize - 1;
    }

    size_t numberOfChar = 0;

    /* If the layout is a subscript or a prefix, write "\x14{indice\x14}".
     * System braces are used to avoid confusion with lists. */
    constexpr size_t k_lengthSystemAndBracket =
        UTF8Decoder::CharSizeOfCodePoint(UCodePointSystem) +
        UTF8Decoder::CharSizeOfCodePoint('{');
    static_assert(UTF8Decoder::CharSizeOfCodePoint('}') ==
                      UTF8Decoder::CharSizeOfCodePoint('{'),
                  "Right and left brackets not the same size");
    if (numberOfChar + k_lengthSystemAndBracket >=
        static_cast<size_t>(bufferSize - 1)) {
      return numberOfChar;
    }

    numberOfChar =
        SerializationHelper::CodePoint(buffer, bufferSize, UCodePointSystem);
    assert(numberOfChar < bufferSize - 1);
    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar, '{');
    assert(numberOfChar < bufferSize - 1);

    numberOfChar +=
        const_cast<VerticalOffsetLayoutNode *>(this)->indiceLayout()->serialize(
            buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode,
            numberOfSignificantDigits);
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }

    if (numberOfChar + k_lengthSystemAndBracket >=
        static_cast<size_t>(bufferSize - 1)) {
      return numberOfChar;
    }
    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar, UCodePointSystem);
    assert(numberOfChar < bufferSize - 1);
    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar, '}');
    assert(numberOfChar < bufferSize - 1);
    return numberOfChar;
  }

  assert(verticalPosition() == VerticalPosition::Superscript);
  // If the layout is a superscript, write: '^' 'System(' indice 'System)'
  size_t numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, '^');
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }
  numberOfChar += SerializationHelper::CodePoint(
      buffer + numberOfChar, bufferSize - numberOfChar,
      UCodePointLeftSystemParenthesis);
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }
  numberOfChar +=
      const_cast<VerticalOffsetLayoutNode *>(this)->indiceLayout()->serialize(
          buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode,
          numberOfSignificantDigits);
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }
  numberOfChar += SerializationHelper::CodePoint(
      buffer + numberOfChar, bufferSize - numberOfChar,
      UCodePointRightSystemParenthesis);
  return std::min(numberOfChar, bufferSize - 1);
}

LayoutNode *VerticalOffsetLayoutNode::baseLayout() {
  LayoutNode *parentNode = parent();
  assert(parentNode != nullptr);
  if (parentNode->otype() != Type::HorizontalLayout) {
    return nullptr;
  }
  int idxInParent = parentNode->indexOfChild(this);
  assert(idxInParent >= 0);
  int baseIndex = idxInParent + baseOffsetInParent();
  if (baseIndex < 0 || baseIndex >= parent()->numberOfChildren()) {
    return nullptr;
  }
  LayoutNode *result = parentNode->childAtIndex(baseIndex);
  if (result->otype() == Type::VerticalOffsetLayout &&
      static_cast<VerticalOffsetLayoutNode *>(result)->horizontalPosition() !=
          horizontalPosition()) {
    /* If two vertical offset layouts, one prefix and one suffix, are next to
     * each other, the size of each one depends on the other one so thay can't
     * rely on their base to draw themselves. */
    return nullptr;
  }
  return result;
}

bool VerticalOffsetLayoutNode::protectedIsIdenticalTo(OLayout l) {
  assert(l.otype() == Type::VerticalOffsetLayout);
  VerticalOffsetLayoutNode *n =
      static_cast<VerticalOffsetLayoutNode *>(l.node());
  return verticalPosition() == n->verticalPosition() &&
         horizontalPosition() == n->horizontalPosition() &&
         LayoutNode::protectedIsIdenticalTo(l);
}

VerticalOffsetLayout VerticalOffsetLayout::Builder(
    OLayout l, VerticalOffsetLayoutNode::VerticalPosition verticalPosition,
    VerticalOffsetLayoutNode::HorizontalPosition horizontalPosition) {
  void *bufferNode = Pool::sharedPool->alloc(sizeof(VerticalOffsetLayoutNode));
  VerticalOffsetLayoutNode *node = new (bufferNode)
      VerticalOffsetLayoutNode(verticalPosition, horizontalPosition);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, l);
  return static_cast<VerticalOffsetLayout &>(h);
}

}  // namespace Poincare
