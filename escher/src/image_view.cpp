#include <escher/image_view.h>
extern "C" {
#include <assert.h>
#include <stdint.h>
}
#include <ion.h>

ImageView::ImageView() :
  View(),
  m_image(nullptr),
  m_data(nullptr),
  m_dataLength(0)
{
}

constexpr static int maxPixelBufferSize = 4000;
// Icon file is 55 x 56 = 3080
// Boot logo file is 188 x 21 = 3948

void ImageView::drawRect(KDContext * ctx, KDRect rect) const {
  const uint8_t* data;
  size_t size;
  size_t pixelBufferSize;
 
  if (m_image != nullptr) {
  
    assert(bounds().width() == m_image->width());
    assert(bounds().height() == m_image->height());
    
    data = m_image->compressedPixelData();
    size = m_image->compressedPixelDataSize();
    pixelBufferSize = m_image->width() * m_image->height();
  } else if (m_data != nullptr) {
    data = m_data;
    size = m_dataLength;
    // We assume the external images are made properly.
    // TODO: Maybe we shouldn't...
    pixelBufferSize = bounds().width() * bounds().height();
  } else {
    return;
  }

  KDColor pixelBuffer[maxPixelBufferSize];
  assert(pixelBufferSize <= maxPixelBufferSize);
  assert(Ion::stackSafe()); // That's a VERY big buffer we're allocating on the stack

  Ion::decompress(
    data,
    reinterpret_cast<uint8_t *>(pixelBuffer),
    size,
    pixelBufferSize * sizeof(KDColor)
  );

  // If we are on a big-endian CPU, we need to swap the bytes
  #if _BIG_ENDIAN
  for (uint32_t i = 0; i < pixelBufferSize; i++) {
    pixelBuffer[i] = KDColor::RGB16(__builtin_bswap16(pixelBuffer[i]));
  }
  #endif

  ctx->fillRectWithPixels(bounds(), pixelBuffer, nullptr);
}

void ImageView::setImage(const uint8_t *data, size_t dataLength) {
  if (data != m_data && dataLength != m_dataLength) {
    m_data = data;
    m_dataLength = dataLength;
    markRectAsDirty(bounds());
  }
}

void ImageView::setImage(const Image * image) {
  if (image != m_image) {
    m_image = image;
    markRectAsDirty(bounds());
  }
}
