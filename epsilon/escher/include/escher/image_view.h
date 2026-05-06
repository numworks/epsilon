#pragma once

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
  /* Icon file is 55 x 56 x 3 (x3 because of transparency) = 9240
   * Boot logo file is 188 x 21 = 3948 */
  constexpr static int k_maxPixelBufferSize = 9300;
  const Image* m_image;
};

}  // namespace Escher
