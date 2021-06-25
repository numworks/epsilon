#ifndef APPS_PROBABILITY_GUI_HIGHLIGHT_IMAGE_CELL_H
#define APPS_PROBABILITY_GUI_HIGHLIGHT_IMAGE_CELL_H

#include <escher/highlight_cell.h>
#include <escher/palette.h>
#include <escher/transparent_image_view.h>

namespace Probability {

class HighlightImageCell : public Escher::TransparentImageView {
public:
  HighlightImageCell() : Escher::TransparentImageView() { setBackgroundColor(KDColorWhite); }
  void setHighlighted(bool highlighted) {
    setBackgroundColor(highlighted ? Escher::Palette::Select : KDColorWhite);
  };
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_HIGHLIGHT_IMAGE_CELL_H */
