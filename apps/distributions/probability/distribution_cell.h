#ifndef DISTRIBUTIONS_PROBABILITY_DISTRIBUTION_CELL_H
#define DISTRIBUTIONS_PROBABILITY_DISTRIBUTION_CELL_H

#include <escher/chevron_view.h>
#include <escher/i18n.h>
#include <escher/message_text_view.h>
#include <escher/table_cell.h>
#include <escher/transparent_image_view.h>

namespace Distributions {

class DistributionCell : public Escher::TableCell {
 public:
  DistributionCell();
  const View* labelView() const override { return &m_iconView; }
  const View* accessoryView() const override { return &m_chevronView; }
  const View* subLabelView() const override { return &m_messageView; }
  bool shouldAlignSublabelRight() const override { return false; }

  void reloadCell() override;
  void setLabel(I18n::Message message);
  void setImage(const Escher::Image* image);

 private:
  constexpr static KDCoordinate k_iconWidth = 35;
  constexpr static KDCoordinate k_iconHeight = 19;
  constexpr static KDCoordinate k_iconMargin = 10;
  constexpr static KDCoordinate k_chevronWidth = 8;
  constexpr static KDCoordinate k_chevronMargin = 10;
  Escher::MessageTextView m_messageView;
  Escher::TransparentImageView m_iconView;
  Escher::ChevronView m_chevronView;
};

}  // namespace Distributions

#endif
