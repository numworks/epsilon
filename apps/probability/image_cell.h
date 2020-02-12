#ifndef PROBABILITY_IMAGE_CELL_H
#define PROBABILITY_IMAGE_CELL_H

#include <escher.h>

namespace Probability {

class ImageCell : public HighlightCell {
public:
  ImageCell();
  void setHighlighted(bool highlight) override;
  void setImage(const Image * image, const Image * focusedImage);
  constexpr static KDCoordinate k_width = 39;
  constexpr static KDCoordinate k_height = 23;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ImageView m_iconView;
  const Image * m_icon;
  const Image * m_focusedIcon;
};

}

#endif
