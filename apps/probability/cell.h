#ifndef APPS_PROBABILITY_CELL_H
#define APPS_PROBABILITY_CELL_H

#include <escher.h>

namespace Probability {

class Cell final : public HighlightCell {
public:
  Cell() :   HighlightCell(), m_labelView(KDText::FontSize::Large, (I18n::Message)0, 0, 0.5, KDColorBlack, KDColorWhite), m_icon(nullptr), m_focusedIcon(nullptr) {}
  void reloadCell() override;
  void setLabel(I18n::Message message) {
    m_labelView.setMessage(message);
  }
  void setImage(const Image * image, const Image * focusedImage) {
    m_icon = image;
    m_focusedIcon = focusedImage;
  }
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_iconWidth = 35;
  constexpr static KDCoordinate k_iconHeight = 19;
  constexpr static KDCoordinate k_iconMargin = 10;
  constexpr static KDCoordinate k_chevronWidth = 8;
  constexpr static KDCoordinate k_chevronMargin = 10;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
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
