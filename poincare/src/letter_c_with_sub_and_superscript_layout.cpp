#include <poincare/letter_c_with_sub_and_superscript_layout.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

const uint8_t symbolUpperHalf[] = {
  0xFF, 0xFF, 0xFF, 0xE9, 0x8D, 0x01, 0x00, 0x00, 0x26, 0x9C, 0xFE, 0xFF,
  0xFF, 0xFF, 0xB7, 0x00, 0x6B, 0xD0, 0xFF, 0xFF, 0xD2, 0x57, 0x59, 0xE8,
  0xFF, 0xDD, 0x00, 0xA9, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0x9C, 0xDC,
  0xFF, 0x42, 0x7D, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
  0xD1, 0x00, 0xDD, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE,
  0x9D, 0x58, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
  0x6B, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE,
  0x6B, 0x8D, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF,
};

int LetterCWithSubAndSuperscriptLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomialCoefficient::s_functionHelper.name(), true);
}

void LetterCWithSubAndSuperscriptLayoutNode::renderLetter(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  uint8_t symbolPixel[k_symbolHeight * k_symbolWidth];
  static_assert(k_symbolHeight%2 == 0, "ncr_layout : k_symbolHeight is odd");

  for (int i = 0; i < k_symbolHeight / 2; i++) {
    for (int j = 0; j < k_symbolWidth; j++) {
      symbolPixel[i * k_symbolWidth + j] = symbolUpperHalf[i * k_symbolWidth + j];
      // The bottom half is obtained by mirroring the upper half
      symbolPixel[(k_symbolHeight - 1 - i) * k_symbolWidth + j] = symbolUpperHalf[i * k_symbolWidth + j];
    }
  }

  KDColor workingBuffer[k_symbolWidth * k_symbolHeight];
  KDRect symbolFrame(p.x() + k_margin, p.y(), k_symbolWidth, k_symbolHeight);

  ctx->blendRectWithMask(symbolFrame, expressionColor, (const uint8_t *)symbolPixel, (KDColor *)workingBuffer);
}

}
