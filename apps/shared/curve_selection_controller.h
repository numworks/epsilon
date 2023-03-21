#ifndef SHARED_CURVE_SELECTION_CONTROLLER
#define SHARED_CURVE_SELECTION_CONTROLLER

#include <escher/chevron_view.h>
#include <escher/expression_view.h>
#include <escher/list_view_data_source.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/view_controller.h>

namespace Shared {

/* Expression view with color on the left
 *
 * TODO: The color indicator should be factorized with Graph::FunctionCell
 * and Shared::FunctionTitleCell.
 */
template <typename Accessory>
class CurveSelectionCellWithAccessory
    : public Escher::MenuCell<Escher::ExpressionView, Escher::EmptyCellWidget,
                              Accessory> {
 public:
  CurveSelectionCellWithAccessory()
      : Escher::MenuCell<Escher::ExpressionView, Escher::EmptyCellWidget,
                         Accessory>(),
        m_color(KDColorBlack) {}
  void drawRect(KDContext *ctx, KDRect rect) const override {
    Escher::MenuCell<Escher::ExpressionView, Escher::EmptyCellWidget,
                     Accessory>::drawRect(ctx, rect);
    // Draw the color indicator
    assert(m_color != KDColorBlack);  // Check that the color was set
    ctx->fillRect(
        KDRect(0, 0, k_colorIndicatorThickness, this->bounds().height()),
        m_color);
  }
  void setColor(KDColor color) { m_color = color; }

 private:
  constexpr static KDCoordinate k_colorIndicatorThickness = 3;
  KDColor m_color;
};

using CurveSelectionCell =
    CurveSelectionCellWithAccessory<Escher::EmptyCellWidget>;
using CurveSelectionCellWithChevron =
    CurveSelectionCellWithAccessory<Escher::ChevronView>;

class InteractiveCurveViewController;

class CurveSelectionController
    : public Escher::SelectableListViewController<Escher::ListViewDataSource> {
 public:
  CurveSelectionController(InteractiveCurveViewController *graphController);

  void viewWillAppear() override { m_selectableListView.reloadData(); }
  bool handleEvent(Ion::Events::Event event) override;
  int typeAtIndex(int index) const override { return k_curveSelectionCellType; }
  CurveSelectionCellWithChevron *reusableCell(int index, int type) override = 0;

 protected:
  constexpr static int k_curveSelectionCellType = 0;
  InteractiveCurveViewController *m_graphController;
};

}  // namespace Shared

#endif
