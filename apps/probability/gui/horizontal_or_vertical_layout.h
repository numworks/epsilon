#ifndef APPS_PROBABILITY_GUI_VERTICAL_LAYOUT_H
#define APPS_PROBABILITY_GUI_VERTICAL_LAYOUT_H

#include <apps/i18n.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/message_text_view.h>
#include <escher/palette.h>
#include <escher/responder.h>
#include <escher/solid_color_view.h>
#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/rect.h>

#include "selectable_cell_list_controller.h"

namespace Probability {

/* View that lays out its subviews vertically.*/
class VerticalLayout : public Escher::SolidColorView {
public:
  VerticalLayout(KDColor color = Palette::WallScreen) : Escher::SolidColorView(color), m_marginX(0), m_marginY(0) {}
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force = false) override;
  void setMargins(KDCoordinate marginX, KDCoordinate marginY) {
    m_marginX = marginX;
    m_marginY = marginY;
  }

private:
  KDCoordinate m_marginX;
  KDCoordinate m_marginY;
};

/* View that lays out its subviews horizontally.*/
class HorizontalLayout : public Escher::SolidColorView {
public:
  HorizontalLayout(KDColor color = Palette::WallScreen) : Escher::SolidColorView(color) {}
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force = false) override;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_VERTICAL_LAYOUT_H */
