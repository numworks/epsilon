#pragma once

#include <escher/layout_preferences.h>

#include "themed_image.h"
#include "view.h"

namespace Escher {

class ThemedImageView : public View {
 public:
  ThemedImageView() = default;
  ThemedImageView(ThemedImage themedImage) : m_themedImage(themedImage) {}

  void setThemedImage(ThemedImage themedImage) {
    if (themedImage != m_themedImage) {
      m_themedImage = themedImage;
      markWholeFrameAsDirty();
    }
  }

  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  const Image* currentImage() const;

 private:
  // TODO: do not duplicate with ImageView
  // Boot logo image is 188 x 21 = 3948
  constexpr static int k_maxImageSize = 188 * 21;

  ThemedImage m_themedImage;
};

}  // namespace Escher
