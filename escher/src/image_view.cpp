#include <escher/image_view.h>
extern "C" {
#include <assert.h>
}

ImageView::ImageView(const Image * image) :
  ChildlessView(),
  m_image(image)
{
}

void ImageView::drawRect(KDContext * ctx, KDRect rect) const {
  assert(bounds().width() == m_image->width());
  assert(bounds().height() == m_image->height());
  ctx->fillRectWithPixels(bounds(), m_image->pixels(), nullptr);
}
