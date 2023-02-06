#include <poincare_junior/include/layout.h>
#include <poincare_junior/include/expression.h>
#include <ion/unicode/code_point.h>
#include <string.h>

namespace PoincareJ {

EditionReference Layout::ParseFromTextInEditionPool(const char * text) {
  int n = std::strlen(text);
  EditionReference ref = EditionReference::Push<BlockType::RackLayout>(n);
  for (int i = 0; i < n; i++) {
    EditionReference::Push<BlockType::CodePointLayout, CodePoint>(text[i]);
  }
  return ref;
}

EditionReference Layout::ParseFromExpressionInEditionPool(Node node) {
  // node == -1+2*3
  EditionReference ref = EditionReference::Push<BlockType::RackLayout>(6);
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('-');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('1');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('+');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('2');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('*');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('3');
  // Remove node from EditionReference
  EditionReference nodeRef(node);
  nodeRef.removeTree();
  return ref;
}

Layout Layout::Parse(const char * textInput) {
  return Layout([](const char * text){
      ParseFromTextInEditionPool(text);
    }, textInput);
}

Layout Layout::ToLayout(const Expression * expressionInput) {
  return Layout([](Node node){
      ParseFromExpressionInEditionPool(node);
    }, expressionInput);
}

void Layout::toText(char * buffer, size_t bufferSize) const {
  memcpy(buffer, "-1+2*3", bufferSize);
}

}
