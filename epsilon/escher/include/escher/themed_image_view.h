#pragma once

#include <escher/layout_preferences.h>

#include "cell_widget.h"
#include "themed_image.h"
#include "view.h"

namespace Escher {

class ThemedImageView : public View, public CellWidget {
 public:
  ThemedImageView() = default;
  ThemedImageView(ThemedImage themedImage) : m_themedImage(themedImage) {}
  const View* view() const override { return this; }

  void setThemedImage(ThemedImage themedImage) {
    if (themedImage != m_themedImage) {
      m_themedImage = themedImage;
      markWholeFrameAsDirty();
    }
  }

  void setSingleImage(const Image* simpleImage) {
    m_themedImage = ThemedImage(simpleImage, simpleImage, simpleImage);
  }

  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  const Image* currentImage() const;

  void setHighlighted(bool highlighted) override {
    m_isHighlighted = highlighted;
  }

 private:
  // Boot logo image is 188 x 21 = 3948
  constexpr static int k_maxImageSize = 188 * 21;

  ThemedImage m_themedImage;
  bool m_isHighlighted = false;
};

}  // namespace Escher
