#ifndef POINCARE_LINEAR_LAYOUT_DECODER_H
#define POINCARE_LINEAR_LAYOUT_DECODER_H

#include <ion/unicode/utf8_decoder.h>

#include "horizontal_layout.h"

namespace Poincare {

/* This helper provides a codepoint string api to HorizontalLayout containing
 * only CodePointLayouts (used in layout fields in linear mode) */

class LinearLayoutDecoder : public UnicodeDecoder {
 public:
  LinearLayoutDecoder(const HorizontalLayout layout, size_t initialPosition = 0,
                      size_t layoutEnd = 0);
  CodePoint nextCodePoint() {
    return nextCodePointAtDirection(OMG::Direction::Right());
  }
  CodePoint previousCodePoint() {
    return nextCodePointAtDirection(OMG::Direction::Left());
  }

 private:
  CodePoint nextCodePointAtDirection(OMG::HorizontalDirection direction);
  const HorizontalLayout m_layout;
  bool m_insideCombinedCodePoint;
};

}  // namespace Poincare

#endif
