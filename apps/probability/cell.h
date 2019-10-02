#ifndef APPS_PROBABILITY_CELL_H
#define APPS_PROBABILITY_CELL_H

#include <escher.h>

namespace Probability {

class Cell : public HighlightCell {
public:
  Cell();
  void reloadCell() override;
  void setLabel(I18n::Message message);
  void setImage(const Image * image, const Image * focusedImage);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_iconWidth = 35;
  constexpr static KDCoordinate k_iconHeight = 19;
  constexpr static KDCoordinate k_iconMargin = 10;
  constexpr static KDCoordinate k_chevronWidth = 8;
  constexpr static KDCoordinate k_chevronMargin = 10;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  MessageTextView m_labelView;
  ImageView m_iconView;
  /* TODO: One day, we would rather store a mask (8bits/pixel) instead of two
   * images (16bits/pixels)*/
  const Image * m_icon;
  const Image * m_focusedIcon;
  ChevronView m_chevronView;
};

}

#endif
