#ifndef SHARED_COLOR_CELL_H
#define SHARED_COLOR_CELL_H

#include <escher/arbitrary_shaped_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

namespace Shared {

/* Not ArbitraryShapedView since the shape never changes, the parent does not
 * need to be invalidated */
class ColorView : public Escher::View, public Escher::CellWidget {
 public:
  const View* view() const override { return this; }
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void setColor(KDColor color);

 private:
  KDColor m_color;
};

using ColorCell = Escher::MenuCell<Escher::MessageTextView,
                                   Escher::EmptyCellWidget, ColorView>;

}  // namespace Shared

#endif
