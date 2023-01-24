#include <poincare/sum_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <algorithm>



namespace Poincare {
// Stores a single branch of the sigma symbol
// Data is stored so that every two line a white pixel must be added. This way the branch's slope is respected
constexpr static int k_significantPixelWidth = 6;
const uint8_t symbolPixelOneBranchLargeFont[((SequenceLayoutNode::SymbolHeight(KDFont::Size::Large)- 1) / 2) * k_significantPixelWidth] = {
  0xCF, 0x10, 0xDF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x70, 0x4D, 0xFF, 0xFF, 0xFF,
  0xEF, 0x10, 0xBF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xAA, 0x20, 0xFF, 0xFF, 0xFF,
  0xFF, 0x4D, 0x7F, 0xFF, 0xFF, 0xFF,
  0xFF, 0xDF, 0x10, 0xDF, 0xFF, 0xFF,
  0xFF, 0x7F, 0x4D, 0xFF, 0xFF, 0xFF,
  0xFF, 0xEF, 0x20, 0xBF, 0xFF, 0xFF,
  0xFF, 0xAA, 0x20, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0x4D, 0x7F, 0xFF, 0xFF,
  0xFF, 0xDF, 0x20, 0xDF, 0xFF, 0xFF,
  0xFF, 0xFF, 0x7F, 0x4D, 0xFF, 0xFF,
  0xFF, 0xFF, 0x30, 0xBF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xAA, 0x30, 0xFF, 0xFF,
};

int SumLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SequenceLayoutNode::writeDerivedClassInBuffer("sum", buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
}

void SumLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  // Creates half size sigma symbol from one branch
  uint8_t symbolPixel[SymbolHeight(font) * SymbolWidth(font)];
  int whiteOffset;

  // Taking care of the first line which is a black straight line at the exception of the first pixel
  symbolPixel[0] = 0x30;
  for (int j = 0; j < SymbolWidth(font); j++) {
    symbolPixel[j] = 0x00;
  }

  static_assert(SymbolHeight(KDFont::Size::Large)%2 != 0 && SymbolHeight(KDFont::Size::Small)%2 != 0, "sum_layout : SymbolHeight is even");
  for (int i = 1; i < (SymbolHeight(font) + 1)/2; i++) {
    // Adding the white offset
    whiteOffset = (i-1)/2;
    for (int j = 0; j < whiteOffset; j++) {
      symbolPixel[i*SymbolWidth(font) + j] = 0xFF;
    }

    // Adding the actual pixels of the branch
    for (int j = 0; j < k_significantPixelWidth; j++) {
      symbolPixel[i*SymbolWidth(font) + whiteOffset + j] = symbolPixelOneBranchLargeFont[(i-1)*k_significantPixelWidth + j];
    }

    // Filling the gap with white
    for (int j = whiteOffset + k_significantPixelWidth  ; j < SymbolWidth(font); j++) {
      symbolPixel[i*SymbolWidth(font) + j] = 0xFF;
    }
  }

  // Create real size sigma symbol by flipping the previous array
  for (int i = SymbolHeight(font)/2 + 1; i < SymbolHeight(font); i++) {
    for (int j = 0; j < SymbolWidth(font); j++) {
      symbolPixel[i*SymbolWidth(font) + j] = symbolPixel[(SymbolHeight(font)-i-1)*SymbolWidth(font) +j];
    }
  }

  // Compute sizes.
  KDSize upperBoundSize = upperBoundLayout()->layoutSize(font);
  KDSize lowerBoundNEqualsSize = lowerBoundSizeWithVariableEquals(font);

  // Render the Sum symbol.
  KDColor workingBuffer[SymbolWidth(font)*SymbolHeight(font)];
  KDRect symbolFrame(
    p.x() + std::max({0, (upperBoundSize.width() - SymbolWidth(font)) / 2, (lowerBoundNEqualsSize.width() - SymbolWidth(font)) / 2}),
    p.y() + std::max(upperBoundSize.height() + UpperBoundVerticalMargin(font), argumentLayout()->baseline(font) - (SymbolHeight(font) + 1) / 2),
    SymbolWidth(font),
    SymbolHeight(font));
  ctx->blendRectWithMask(symbolFrame, expressionColor, (const uint8_t *)symbolPixel, (KDColor *)workingBuffer);

  // Render the "n=" and the parentheses.
  SequenceLayoutNode::render(ctx, p, font, expressionColor, backgroundColor);
}
}
