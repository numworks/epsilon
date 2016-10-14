#include <escher/image_view.h>
extern "C" {
#include <assert.h>
}

ImageView::ImageView() :
  View(),
  m_image(nullptr)
{
}

void ImageView::drawRect(KDContext * ctx, KDRect rect) const {
  if (m_image == nullptr) {
    return;
  }
  assert(bounds().width() == m_image->width());
  assert(bounds().height() == m_image->height());
  ctx->fillRectWithPixels(bounds(), m_image->pixels(), nullptr);
}

void ImageView::setImage(const Image * image) {
  if (image != m_image) {
    m_image = image;
    markRectAsDirty(bounds());
  }
}
