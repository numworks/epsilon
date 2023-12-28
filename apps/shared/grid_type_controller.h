#ifndef SHARED_GRID_TYPE_CONTROLLER_H
#define SHARED_GRID_TYPE_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_list_view_delegate.h>
#include <escher/stack_view_controller.h>
#include <escher/transparent_image_view.h>

#include "interactive_curve_view_range.h"

namespace Shared {

class GridTypeController : public Escher::ExplicitSelectableListViewController,
                           public Escher::SelectableListViewDelegate {
 public:
  using GridType = InteractiveCurveViewRange::GridType;
  GridTypeController(Escher::Responder *parentResponder,
                     InteractiveCurveViewRange *interactiveCurveViewRange);

  void viewWillAppear() override;

  bool handleEvent(Ion::Events::Event event) override;
  Escher::StackViewController *stackController() {
    return static_cast<Escher::StackViewController *>(parentResponder());
  }

  const char *title() override;

  int numberOfRows() const override { return 2; }
  Escher::HighlightCell *cell(int row) override {
    assert(row >= 0 & row < 2);
    return &m_cells[row];
  }

 private:
  using MenuCell =
      Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                       Escher::TransparentImageView>;
  MenuCell m_cells[2];
  InteractiveCurveViewRange *m_viewRange;
};

}  // namespace Shared

#endif