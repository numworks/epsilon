#ifndef ESCHER_HIGHLIGHT_IMAGE_CELL_H
#define ESCHER_HIGHLIGHT_IMAGE_CELL_H

#include <escher/highlight_cell.h>
#include <escher/transparent_image_view.h>

namespace Escher {

/* Highlightable ImageCell (changes background color according to selection) */
class HighlightImageCell : public HighlightCell {
 public:
  HighlightImageCell();
  void setHighlighted(bool highlighted) override;

  // Wrap TransparentImageView
  int numberOfSubviews() const override { return 1; }
  View* subviewAtIndex(int i) override { return &m_contentView; }
  KDSize minimalSizeForOptimalDisplay() const override;
  void setImage(const Image* image) { m_contentView.setImage(image); }

 private:
  constexpr static KDColor k_unselectedBackgroundColor = KDColorWhite;
  void layoutSubviews(bool force) override;
  TransparentImageView m_contentView;
};

}  // namespace Escher

#endif
