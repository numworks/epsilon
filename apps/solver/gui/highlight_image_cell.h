#ifndef SOLVER_GUI_HIGHLIGHT_IMAGE_CELL_H
#define SOLVER_GUI_HIGHLIGHT_IMAGE_CELL_H

// TODO Hugo : Factorize with probability

#include <escher/highlight_cell.h>
#include <escher/transparent_image_view.h>

namespace Solver {

/* Highlightable ImageCell (changes background color according to selection) */
class HighlightImageCell : public Escher::HighlightCell {
public:
  HighlightImageCell();
  void setHighlighted(bool highlighted) override;

  // Wrap TransparentImageView
  int numberOfSubviews() const override { return 1; }
  Escher::View * subviewAtIndex(int i) override { return &m_contentView; }
  KDSize minimalSizeForOptimalDisplay() const override;
  void setImage(const Escher::Image * image) { m_contentView.setImage(image); }

private:
  constexpr static KDColor k_unselectedBackgroundColor = KDColorWhite;
  void layoutSubviews(bool force) override;
  Escher::TransparentImageView m_contentView;
};

}  // namespace Solver

#endif /* SOLVER_GUI_HIGHLIGHT_IMAGE_CELL_H */
