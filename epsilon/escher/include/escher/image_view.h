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
  const Image* m_image;

 private:
  // Boot logo image is 188 x 21 = 3948
  constexpr static int k_maxImageSize = 188 * 21;
};

}  // namespace Escher
