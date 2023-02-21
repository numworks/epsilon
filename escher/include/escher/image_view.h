#ifndef ESCHER_IMAGE_VIEW_H
#define ESCHER_IMAGE_VIEW_H

#include <escher/image.h>
#include <escher/view.h>

namespace Escher {

class ImageView : public View {
 public:
  ImageView();
  void setImage(const Image* image);
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_image ? KDSize(m_image->width(), m_image->height()) : KDSizeZero;
  }

 protected:
  // Icon file is 55 x 56 = 3080
  // Boot logo file is 188 x 21 = 3948
  constexpr static int k_maxPixelBufferSize = 4000;
  const Image* m_image;
};

}  // namespace Escher

#endif
