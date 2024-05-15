#include <escher/image_view.h>
extern "C" {
#include <assert.h>
}
#include <omg/memory.h>

namespace Escher {

ImageView::ImageView() : View(), m_image(nullptr) {}

void ImageView::drawRect(KDContext *ctx, KDRect rect) const {
  if (m_image == nullptr) {
    return;
  }
  assert(bounds().width() == m_image->width());
  assert(bounds().height() == m_image->height());

  KDColor pixelBuffer[k_maxPixelBufferSize];
  int pixelBufferSize = m_image->width() * m_image->height();
  assert(pixelBufferSize <= k_maxPixelBufferSize);
  // That's a VERY big buffer we're allocating on the stack
  assert(Ion::stackSafe());

  OMG::Memory::Decompress(
      m_image->compressedPixelData(), reinterpret_cast<uint8_t *>(pixelBuffer),
      m_image->compressedPixelDataSize(), pixelBufferSize * sizeof(KDColor));

  ctx->fillRectWithPixels(bounds(), pixelBuffer, nullptr);
}

void ImageView::setImage(const Image *image) {
  if (image != m_image) {
    m_image = image;
    markWholeFrameAsDirty();
  }
}

}  // namespace Escher
