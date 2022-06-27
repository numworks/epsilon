#include <escher/icon_view.h>
extern "C" {
#include <assert.h>
}
#include <ion.h>
#include <kandinsky.h>

IconView::IconView() :
  View(),
  m_image(nullptr)
{
}

constexpr static int iconBufferSize = 3080; 
// Icon file is 55 x 56 = 3080

void IconView::drawRect(KDContext * ctx, KDRect rect) const {
  const uint8_t* data;
  size_t size;

  if (m_image != nullptr) {
    assert(bounds().width() == m_image->width());
    assert(bounds().height() == m_image->height());
    assert(bounds().width() == 55);
    assert(bounds().height() == 56);
    
    data = m_image->compressedPixelData();
    size = m_image->compressedPixelDataSize();
  } else if (m_data != nullptr) {
    data = m_data;
    size = m_dataLength;
  } else {
    return;
  }

  KDColor pixelBuffer[iconBufferSize];
  assert(Ion::stackSafe()); // That's a VERY big buffer we're allocating on the stack

  Ion::decompress(
    data,
    reinterpret_cast<uint8_t *>(pixelBuffer),
    size,
    iconBufferSize * sizeof(KDColor)
  );

  //We push the first 6 lines of the image so that they are truncated on the sides
  ctx->fillRectWithPixels(KDRect(6, 0, m_frame.width()-12, 1),pixelBuffer+6, nullptr);
  ctx->fillRectWithPixels(KDRect(4, 1, m_frame.width()-8, 1),pixelBuffer+4+55, nullptr);
  ctx->fillRectWithPixels(KDRect(3, 2, m_frame.width()-6, 1),pixelBuffer+3+(2*55), nullptr);
  ctx->fillRectWithPixels(KDRect(2, 3, m_frame.width()-4, 1),pixelBuffer+2+(3*55), nullptr);
  ctx->fillRectWithPixels(KDRect(1, 4, m_frame.width()-2, 1),pixelBuffer+1+(4*55), nullptr);
  ctx->fillRectWithPixels(KDRect(1, 5, m_frame.width()-2, 1),pixelBuffer+1+(5*55), nullptr);
  
  //Then we push the rectangular part of the image
  ctx->fillRectWithPixels(KDRect(0, 6, m_frame.width(), 44),pixelBuffer+(6*55), nullptr);

  //Finally we push the last 5 lines of the image so that they are truncated on the sides 
  ctx->fillRectWithPixels(KDRect(1, 50, m_frame.width()-2, 1),pixelBuffer+1+(50*55), nullptr);
  ctx->fillRectWithPixels(KDRect(1, 51, m_frame.width()-2, 1),pixelBuffer+1+(51*55), nullptr);
  ctx->fillRectWithPixels(KDRect(2, 52, m_frame.width()-4, 1),pixelBuffer+2+(52*55), nullptr);
  ctx->fillRectWithPixels(KDRect(3, 53, m_frame.width()-6, 1),pixelBuffer+3+(53*55), nullptr);
  ctx->fillRectWithPixels(KDRect(4, 54, m_frame.width()-8, 1),pixelBuffer+4+(54*55), nullptr);
  ctx->fillRectWithPixels(KDRect(6, 55, m_frame.width()-12, 1),pixelBuffer+6+(55*55), nullptr);
}

void IconView::setImage(const uint8_t *data, size_t dataLength) {
  if (data != m_data && dataLength != m_dataLength) {
    m_data = data;
    m_dataLength = dataLength;
    markRectAsDirty(bounds());
  }
}

void IconView::setImage(const Image * image) {
  if (image != m_image) {
    m_image = image;
    markRectAsDirty(bounds());
  }
}
