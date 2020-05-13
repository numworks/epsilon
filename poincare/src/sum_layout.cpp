#include <poincare/sum_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <algorithm>



namespace Poincare {
// Stores a single branch of the sigma symbol
// Data is stored so that every two line a white pixel must be added. This way the branch's slope is respected
constexpr static int k_significantPixelWidth = 6;
const uint8_t symbolPixelOneBranch[((SumLayoutNode::k_symbolHeight-1)/2)*k_significantPixelWidth] = {
  0xF1, 0x21, 0xB2, 0xFF, 0xFF, 0xFF,
  0xFF, 0xC5, 0x18, 0xE7, 0xFF, 0xFF,
  0xFF, 0x7F, 0x46, 0xFE, 0xFF, 0xFF,
  0xFF, 0xFC, 0x39, 0x8E, 0xFF, 0xFF,
  0xFF, 0xDF, 0x15, 0xD0, 0xFF, 0xFF,
  0xFF, 0xFF, 0xA4, 0x2A, 0xF6, 0xFF,
  0xFF, 0xFF, 0x5A, 0x69, 0xFF, 0xFF,
  0xFF, 0xFF, 0xF1, 0x21, 0xB2, 0xFF,
  0xFF, 0xFF, 0xC5, 0x18, 0xE7, 0xFF,
  0xFF, 0xFF, 0xFF, 0x7F, 0x46, 0xFE,
  0xFF, 0xFF, 0xFC, 0x3A, 0x8E, 0xFF,
  0xFF, 0xFF, 0xFF, 0xDF, 0x15, 0xD0,
  0xFF, 0xFF, 0xFF, 0xA0, 0x20, 0xF6,
  0xFF, 0xFF, 0XFF, 0xFF, 0xAB, 0x28,
};

int SumLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SequenceLayoutNode::writeDerivedClassInBuffer("sum", buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
}

void SumLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  // Creates half size sigma symbol from one branch
  uint8_t symbolPixel[k_symbolHeight * k_symbolWidth];
  int whiteOffset;

  // Taking care of the first line which is a black straight line
  for (int j = 0; j < k_symbolWidth; j++) {
    symbolPixel[j] = 0x00;
  }
  static_assert(k_symbolHeight%2 != 0, "sum_layout : k_symbolHeight is even");
  for (int i = 1; i < (k_symbolHeight + 1)/2; i++) {
    // Adding the white offset
    whiteOffset = (i-1)/2;
    for (int j = 0; j < whiteOffset; j++) {
      symbolPixel[i*k_symbolWidth + j] = 0xFF;
    }

    // Adding the actual pixels of the branch
    for (int j = 0; j < k_significantPixelWidth; j++) {
      symbolPixel[i*k_symbolWidth + whiteOffset + j] = symbolPixelOneBranch[(i-1)*k_significantPixelWidth + j];
    }

    // Filling the gap with white
    for (int j = whiteOffset + k_significantPixelWidth  ; j < k_symbolWidth; j++) {
      symbolPixel[i*k_symbolWidth + j] = 0xFF;
    }
  }

  // Create real size sigma symbol by flipping the previous array
  for (int i = k_symbolHeight/2 + 1; i < k_symbolHeight; i++) {
    for (int j = 0; j < k_symbolWidth; j++) {
      symbolPixel[i*k_symbolWidth + j] = symbolPixel[(k_symbolHeight-i-1)*k_symbolWidth +j];
    }
  }

  // Compute sizes.
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDSize lowerBoundNEqualsSize = lowerBoundSizeWithVariableEquals();

  // Render the Sum symbol.
  KDColor workingBuffer[k_symbolWidth*k_symbolHeight];
  KDRect symbolFrame(p.x() + std::max(std::max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundNEqualsSize.width()-k_symbolWidth)/2),
      p.y() + std::max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
      k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(symbolFrame, expressionColor, (const uint8_t *)symbolPixel, (KDColor *)workingBuffer);

  // Render the "n=" and the parentheses.
  SequenceLayoutNode::render(ctx, p, expressionColor, backgroundColor);
}
}
