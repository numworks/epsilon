#ifndef ESCHER_TRANSPARENT_IMAGE_VIEW_H
#define ESCHER_TRANSPARENT_IMAGE_VIEW_H

#include "cell_widget.h"
#include "image_view.h"

namespace Escher {

class TransparentImageView : public ImageView, public CellWidget {
 public:
  const View* view() const override { return this; }
  KDColor backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(KDColor backgroundColor) override {
    m_backgroundColor = backgroundColor;
  }
  void drawRect(KDContext* ctx, KDRect rect) const override;
  bool preventRightAlignedSubLabel(Type type) const override {
    return type == Type::Label;
  }

 private:
  void blendInPlace(KDColor* colorBuffer, uint8_t* alphaBuffer) const;
  KDColor m_backgroundColor;
};

}  // namespace Escher

#endif
