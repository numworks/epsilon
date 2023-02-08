#include <poincare_junior/include/layout.h>
#include <poincare_junior/include/expression.h>
#include <ion/unicode/code_point.h>
#include <string.h>

namespace PoincareJ {

EditionReference Layout::EditionPoolTextToLayout(const char * text) {
  int n = std::strlen(text);
  EditionReference ref = EditionReference::Push<BlockType::RackLayout>(n);
  for (int i = 0; i < n; i++) {
    EditionReference::Push<BlockType::CodePointLayout, CodePoint>(text[i]);
  }
  return ref;
}

EditionReference Layout::EditionPoolLayoutToExpression(Node node) {
  // node == (1-2)/3/4
  EditionReference ref = EditionReference::Push<BlockType::Division>();
  EditionReference::Push<BlockType::Division>();
  EditionReference::Push<BlockType::Subtraction>();
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(4));
  // Remove node from EditionReference
  EditionReference nodeRef(node);
  nodeRef.removeTree();
  return ref;
}

Layout Layout::Parse(const char * textInput) {
  return Layout([](const char * text) {
      EditionPoolTextToLayout(text);
    }, textInput);
}

Expression Layout::toExpression() const {
  return Expression([](Node node) {
      EditionPoolLayoutToExpression(node);
    }, this);
}

void Layout::toText(char * buffer, size_t bufferSize) const {
  memcpy(buffer, "-1+2*3", bufferSize);
}

}
