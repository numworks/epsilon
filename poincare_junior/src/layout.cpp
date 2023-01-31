#include <poincare_junior/include/layout.h>
#include <poincare_junior/include/expression.h>
#include <ion/unicode/code_point.h>
#include <string.h>

namespace PoincareJ {

EditionReference Layout::ParseFromTextInEditionPool(const char * text) {
  // textInput == -1+2*3
  EditionReference ref = EditionReference::Push<BlockType::RackLayout>(6);
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('-');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('1');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('+');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('2');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('*');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('3');
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
  return ref;
}

Layout Layout::CreateLayoutFromText(const char * textInput) {
  return Layout([](const char * text){
      ParseFromTextInEditionPool(text);
    }, textInput);
}

Layout Layout::CreateLayoutFromExpression(const Expression * expressionInput) {
  return Layout([](Node node){
      ParseFromExpressionInEditionPool(node);
    }, expressionInput);
}

void Layout::layoutToBuffer(char * buffer, size_t bufferSize) const {
  memcpy(buffer, "-1+2*3", bufferSize);
}

}
