#ifndef ESCHER_IMAGE_VIEW_H
#define ESCHER_IMAGE_VIEW_H

#include <escher/view.h>
#include <escher/image.h>

namespace Escher {

class ImageView : public View {
public:
  ImageView();
  void setImage(const Image * image);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override { return KDSize(m_image->width(), m_image->height()); }
private:
  const Image * m_image;
};

}

#endif
