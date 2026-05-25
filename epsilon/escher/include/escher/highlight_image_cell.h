#pragma once

#include <escher/highlight_cell.h>
#include <escher/transparent_themed_image_view.h>

namespace Escher {

/* Highlightable ImageCell (changes background color according to selection) */
class HighlightImageCell : public HighlightCell {
 public:
  HighlightImageCell();
  void setHighlighted(bool highlighted) override;

  // Wrap TransparentThemedImageView
  int numberOfSubviews() const override { return 1; }
  View* subviewAtIndex(int i) override { return &m_contentView; }
  KDSize minimalSizeForOptimalDisplay() const override;
  void setThemedImage(ThemedImage image) {
    m_contentView.setThemedImage(image);
  }

 private:
  constexpr static Escher::ThemeComponent k_unselectedBackgroundColor =
      Escher::ThemeComponent::BackgroundWhite;
  void layoutSubviews(bool force) override;
  TransparentThemedImageView m_contentView;
};

}  // namespace Escher
