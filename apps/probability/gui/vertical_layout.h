#ifndef APPS_PROBABILITY_GUI_VERTICAL_LAYOUT_H
#define APPS_PROBABILITY_GUI_VERTICAL_LAYOUT_H

#include <apps/i18n.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/view.h>
#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/rect.h>
#include <escher/palette.h>

#include "selectable_cell_list_controller.h"

namespace Probability {

/*
 * View that lays out its subviews vertically.
 */
class VerticalLayout : public Escher::View {
public:
  VerticalLayout() : m_backgroundColor(Escher::Palette::WallScreen) {}
  void layoutSubviews(bool force = false) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDColor backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(KDColor backgroundColor) { m_backgroundColor = backgroundColor; }

private:
  KDColor m_backgroundColor;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_VERTICAL_LAYOUT_H */
