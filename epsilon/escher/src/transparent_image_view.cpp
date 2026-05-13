#include <escher/theme/color_picker.h>
#include <escher/theme/highlight.h>
#include <escher/transparent_image_view.h>
#include <ion.h>
#include <omg/memory.h>

namespace Escher {

void TransparentImageView::drawRect(KDContext* ctx, KDRect rect) const {
  if (m_image == nullptr) {
    return;
  }
  if (!m_image->hasTransparency()) {
    ImageView::drawRect(ctx, rect);
    return;
  }
  /* Draw transparent image, blended with background color
   * We could also implement blending with the screen with
   * ctx->blendRectWithMask */

  assert(bounds().width() == m_image->width());
  assert(bounds().height() == m_image->height());

  int imageSize = m_image->width() * m_image->height();
  assert(imageSize <= k_maxTransparentImageSize);

  /* The pixel buffer is a concatenation of the pixel colors, which are encoded
   * on two bytes per pixel (the size of KDColor), and then the pixel
   * transparencies, which are encoded on 1 byte per pixel */
  constexpr int k_maxSizeOfPixelBuffer =
      k_maxTransparentImageSize * (sizeof(KDColor) + sizeof(uint8_t));
  /* Note: "/" is an integer division so make sure that k_maxSizeOfPixelBuffer
   * is an even integer */
  KDColor pixelBuffer[k_maxSizeOfPixelBuffer / sizeof(KDColor)];

  int sizeOfPixelBuffer = imageSize * (sizeof(KDColor) + sizeof(uint8_t));

  // That's a VERY big buffer we're allocating on the stack
  assert(Ion::stackSafe());

  OMG::Memory::Decompress(
      m_image->compressedPixelData(), reinterpret_cast<uint8_t*>(pixelBuffer),
      m_image->compressedPixelDataSize(), sizeOfPixelBuffer);

  uint8_t* mask =
      reinterpret_cast<uint8_t*>(pixelBuffer) + sizeof(KDColor) * imageSize;
  blendInPlace(pixelBuffer, mask);

  ctx->fillRectWithPixels(bounds(), pixelBuffer, nullptr);
}

void TransparentImageView::blendInPlace(KDColor* colorBuffer,
                                        uint8_t* alphaBuffer) const {
  int position;
  KDColor backgroundColor = ColorPicker::Resolve(m_backgroundColor);
  for (int row = 0; row < m_image->height(); row++) {
    for (int col = 0; col < m_image->width(); col++) {
      position = col + row * m_image->width();
      colorBuffer[position] = KDColor::Blend(
          colorBuffer[position], backgroundColor, alphaBuffer[position]);
    }
  }
}

void TransparentImageView::setHighlighted(bool highlighted) {
  setBackgroundColor(Highlight(k_defaultBackground, highlighted));
}

}  // namespace Escher
