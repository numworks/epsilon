#ifndef ESCHER_TRANSPARENT_IMAGE_VIEW_H
#define ESCHER_TRANSPARENT_IMAGE_VIEW_H

#include "image_view.h"

namespace Escher {

class TransparentImageView : public ImageView {
 public:
  KDColor backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(const KDColor& backgroundColor) {
    m_backgroundColor = backgroundColor;
  }
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  void blendInPlace(KDColor* colorBuffer, uint8_t* alphaBuffer) const;
  KDColor m_backgroundColor;
};

}  // namespace Escher

#endif
