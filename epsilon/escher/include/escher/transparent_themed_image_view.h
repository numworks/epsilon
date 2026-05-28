#pragma once

#include <escher/theme/components.h>

#include "themed_image_view.h"

namespace Escher {

class TransparentThemedImageView : public ThemedImageView {
 public:
  using ThemedImageView::ThemedImageView;
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
  ThemeComponent m_backgroundColor = ThemeComponent::DefaultBackground;
};

}  // namespace Escher
