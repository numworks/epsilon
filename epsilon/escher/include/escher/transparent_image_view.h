#pragma once

#include <escher/theme/components.h>

#include "cell_widget.h"
#include "image_view.h"
#include "menu_cell.h"

namespace Escher {

class TransparentImageView : public ImageView, public CellWidget {
 public:
  const View* view() const override { return this; }
  ThemeComponent backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(ThemeComponent backgroundColor) {
    m_backgroundColor = backgroundColor;
  }
  void drawRect(KDContext* ctx, KDRect rect) const override;
  bool preventRightAlignedSubLabel(Type type) const override {
    return type == Type::Label;
  }
  void setHighlighted(bool highlighted) override;

 private:
  // App icon images are 55 x 56 = 3080
  constexpr static int k_maxTransparentImageSize = 55 * 56;

  void blendInPlace(KDColor* colorBuffer, uint8_t* alphaBuffer) const;
  ThemeComponent m_backgroundColor = ThemeComponent::BackgroundWhite;
};

}  // namespace Escher
