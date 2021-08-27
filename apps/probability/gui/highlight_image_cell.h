#ifndef APPS_PROBABILITY_GUI_HIGHLIGHT_IMAGE_CELL_H
#define APPS_PROBABILITY_GUI_HIGHLIGHT_IMAGE_CELL_H

#include <escher/highlight_cell.h>
#include <escher/palette.h>
#include <escher/transparent_image_view.h>

namespace Probability {

class HighlightImageCell : public Escher::HighlightCell {
public:
  HighlightImageCell();
  void setHighlighted(bool highlighted) override;

  // Wrap TransparentImageView
  int numberOfSubviews() const override { return 1; }
  Escher::View * subviewAtIndex(int i) override { return &m_contentView; }
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force) override;
  void setImage(const Escher::Image * image) { m_contentView.setImage(image); }

private:
  Escher::TransparentImageView m_contentView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_HIGHLIGHT_IMAGE_CELL_H */
