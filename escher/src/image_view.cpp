#include <escher/image_view.h>
extern "C" {
#include <assert.h>
}
#include <ion.h>

ImageView::ImageView() :
  View(),
  m_image(nullptr)
{
}

constexpr static int maxPixelBufferSize = 4000;
// Icon file is 55 x 56 = 3080
// Boot logo file is 188 x 21 = 3948

void ImageView::drawRect(KDContext * ctx, KDRect rect) const {
  if (m_image == nullptr) {
    return;
  }
  assert(bounds().width() == m_image->width());
  assert(bounds().height() == m_image->height());

  KDColor pixelBuffer[maxPixelBufferSize];
  int pixelBufferSize = m_image->width() * m_image->height();
  assert(pixelBufferSize <= maxPixelBufferSize);
  assert(Ion::stackSafe()); // That's a VERY big buffer we're allocating on the stack

  Ion::decompress(
    m_image->compressedPixelData(),
    reinterpret_cast<uint8_t *>(pixelBuffer),
    m_image->compressedPixelDataSize(),
    pixelBufferSize * sizeof(KDColor)
  );

  ctx->fillRectWithPixels(bounds(), pixelBuffer, nullptr);
}

void ImageView::setImage(const Image * image) {
  if (image != m_image) {
    m_image = image;
    markRectAsDirty(bounds());
  }
}
