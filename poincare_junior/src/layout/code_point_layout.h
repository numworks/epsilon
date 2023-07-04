#ifndef POINCARE_JUNIOR_LAYOUT_CODE_POINT_LAYOUT_H
#define POINCARE_JUNIOR_LAYOUT_CODE_POINT_LAYOUT_H

#include <ion/unicode/code_point.h>
#include <omgpj/bit.h>

#include "render.h"

namespace PoincareJ {

class CodePointLayout {
 public:
  constexpr static uint8_t SubCodePointLayoutAtIndex(CodePoint value,
                                                     int index) {
    return Bit::getByteAtIndex(value, index);
  }
  static CodePoint GetCodePoint(const Node* node);
  static void GetName(const Node* node, char* buffer, size_t bufferSize);
  static KDSize Size(const Node* node, const Node* root, KDFont::Size font);
  static KDCoordinate Baseline(const Node* node, const Node* root,
                               KDFont::Size font);
  static void RenderNode(const Node* node, const Node* root, KDContext* ctx,
                         KDPoint p, KDFont::Size font,
                         KDColor expressionColor = KDColorBlack,
                         KDColor backgroundColor = KDColorWhite);

 private:
  constexpr static const int k_middleDotWidth = 5;
};

}  // namespace PoincareJ

#endif
