#ifndef APPS_PROBABILITY_CELL_H
#define APPS_PROBABILITY_CELL_H

#include <escher/chevron_view.h>
#include <escher/i18n.h>
#include <escher/image_view.h>
#include <escher/message_text_view.h>
#include <escher/table_cell.h>

namespace Probability {

// TODO Hugo : Move the accessory to the left of text
class Cell : public Escher::TableCell {
public:
  Cell();
  View * labelView() const override { return (View *)&m_labelView; }
  View * subLabelView() const override { return (View *)&m_iconView; }
  View * accessoryView() const override { return (View *)&m_chevronView; }

  void reloadCell() override;
  void setLabel(I18n::Message message);
  void setImage(const Escher::Image * image, const Escher::Image * focusedImage);
private:
  constexpr static KDCoordinate k_iconWidth = 35;
  constexpr static KDCoordinate k_iconHeight = 19;
  constexpr static KDCoordinate k_iconMargin = 10;
  constexpr static KDCoordinate k_chevronWidth = 8;
  constexpr static KDCoordinate k_chevronMargin = 10;
  Escher::MessageTextView m_labelView;
  Escher::ImageView m_iconView;
  /* TODO: One day, we would rather store a mask (8bits/pixel) instead of two
   * images (16bits/pixels)*/
  const Escher::Image * m_icon;
  const Escher::Image * m_focusedIcon;
  Escher::ChevronView m_chevronView;
};

}

#endif
