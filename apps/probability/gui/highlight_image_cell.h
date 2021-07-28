#ifndef APPS_PROBABILITY_GUI_HIGHLIGHT_IMAGE_CELL_H
#define APPS_PROBABILITY_GUI_HIGHLIGHT_IMAGE_CELL_H

#include <escher/highlight_cell.h>
#include <escher/palette.h>
#include <escher/transparent_image_view.h>

namespace Probability {

class HighlightImageCell : public Escher::HighlightCell {
public:
  HighlightImageCell() : Escher::HighlightCell() { m_contentView.setBackgroundColor(KDColorWhite); }
  void setHighlighted(bool highlighted) override {
    m_contentView.setBackgroundColor(highlighted ? Escher::Palette::Select : KDColorWhite);
    HighlightCell::setHighlighted(highlighted);
  };

  // Wrap TransparentImageView
  int numberOfSubviews() const override { return 1; }
  Escher::View * subviewAtIndex(int i) override { return &m_contentView; }
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_contentView.minimalSizeForOptimalDisplay();
  }
  void layoutSubviews(bool force) override { m_contentView.setFrame(bounds(), force); }
  void setImage(const Escher::Image * image) { m_contentView.setImage(image); }

private:
  Escher::TransparentImageView m_contentView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_HIGHLIGHT_IMAGE_CELL_H */
