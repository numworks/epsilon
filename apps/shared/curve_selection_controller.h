#ifndef SHARED_CURVE_SELECTION_CONTROLLER
#define SHARED_CURVE_SELECTION_CONTROLLER

#include <escher/chevron_view.h>
#include <escher/expression_view.h>
#include <escher/list_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/table_cell.h>
#include <escher/view_controller.h>

namespace Shared {

/* Expression view with color on the left
 *
 * TODO: The color indicator should be factorized with Graph::FunctionCell
 * and Shared::FunctionTitleCell.
 * Some behaviour could also be factorized with Escher::ExpressionTableCell.
 * More globally, we need a simple way to add widgets and properties to cells.
 * Here the problem is that the color indicator can't be set as an accessory,
 * since there is already a chevron in some cases, and the class can't inherit
 * from ExpressionTableCell because it is not an EvenOddCell, and because the
 * ExpressionView is not Scrollable.
 */
class CurveSelectionCell : public Escher::TableCell {
public:
  CurveSelectionCell() :
    Escher::TableCell(),
    m_expressionView(KDContext::k_alignLeft, KDContext::k_alignCenter, KDColorBlack, KDColorWhite),
    m_color(KDColorBlack)
  {}
  Escher::View * labelView() const override { return const_cast<Escher::ExpressionView *>(&m_expressionView); }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) { m_color = color; }
  void setLayout(Poincare::Layout layout);
private:
  constexpr static KDCoordinate k_colorIndicatorThickness = 3;

  Escher::ExpressionView m_expressionView;
  KDColor m_color;
};

class InteractiveCurveViewController;

class CurveSelectionController : public Escher::ViewController, public Escher::SelectableTableViewDataSource, public Escher::ListViewDataSource {
public:
  CurveSelectionController(InteractiveCurveViewController * graphController);

  Escher::View * view() override { return &m_selectableTableView; }
  void viewWillAppear() override { m_selectableTableView.reloadData(); }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event & event) override;

protected:
  // Add chevron
  class CurveSelectionCellWithChevron : public CurveSelectionCell {
  public:
    CurveSelectionCellWithChevron() : CurveSelectionCell() {}
    Escher::View * accessoryView() const override { return const_cast<Escher::ChevronView *>(&m_chevronView); }
    bool subviewsCanOverlap() const override { return true; }
  private:
    Escher::ChevronView m_chevronView;
  };

  InteractiveCurveViewController * m_graphController;
  Escher::SelectableTableView m_selectableTableView;
};

}

#endif
