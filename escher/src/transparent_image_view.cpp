#include <escher/transparent_image_view.h>

#include <ion.h>

namespace Escher {

void TransparentImageView::drawRect(KDContext * ctx, KDRect rect) const {
  if (!m_image->hasTransparency()) {
    ImageView::drawRect(ctx, rect);
  } else {
    // Draw transparent image, blended with background color
    // We could also implement blending with the screen with ctx->blendRectWithMask

    if (m_image == nullptr) {
      return;
    }
    assert(bounds().width() == m_image->width());
    assert(bounds().height() == m_image->height());

    KDColor pixelBuffer[maxPixelBufferSize];
    int pixelBufferSize =
        m_image->width() * m_image->height() *
        (m_image->hasTransparency() ? sizeof(KDColor) + sizeof(uint8_t) : sizeof(KDColor));

    assert(pixelBufferSize <= maxPixelBufferSize);
    assert(Ion::stackSafe());  // That's a VERY big buffer we're allocating on the stack

    Ion::decompress(m_image->compressedPixelData(),
                    reinterpret_cast<uint8_t *>(pixelBuffer),
                    m_image->compressedPixelDataSize(),
                    pixelBufferSize);

    uint8_t * mask = reinterpret_cast<uint8_t *>(pixelBuffer) +
                     sizeof(KDColor) * m_image->width() * m_image->height();
    blendInplace(pixelBuffer, mask);

    ctx->fillRectWithPixels(bounds(), pixelBuffer, nullptr);
  }
}

void TransparentImageView::blendInplace(KDColor * colorBuffer, uint8_t * alphaBuffer) const {
  int position;
  for (int row = 0; row < m_image->height(); row++) {
    for (int col = 0; col < m_image->width(); col++) {
      position = col + row * m_image->width();
      colorBuffer[position] =
          KDColor::blend(colorBuffer[position], m_backgroundColor, alphaBuffer[position]);
    }
  }
}

}  // namespace Escher
