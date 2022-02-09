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

class InteractiveCurveViewController;

class CurveSelectionController : public Escher::ViewController, public Escher::SelectableTableViewDataSource, public Escher::ListViewDataSource {
public:
  CurveSelectionController(InteractiveCurveViewController * graphController);

  Escher::View * view() override { return &m_selectableTableView; }
  void viewWillAppear() override { m_selectableTableView.reloadData(); }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  int typeAtLocation(int i, int j) override { return 0; }

protected:
  // ExpressionTable cell with chevron and color indicator
  class CurveSelectionCell : public Escher::TableCell {
  public:
    CurveSelectionCell() :
      Escher::TableCell(),
      m_expressionView(KDContext::k_alignLeft, KDContext::k_alignCenter, KDColorBlack, KDColorWhite)
    {}
    Escher::View * labelView() const override { return const_cast<Escher::ExpressionView *>(&m_expressionView); }
    Escher::View * accessoryView() const override { return const_cast<Escher::ChevronView *>(&m_chevronView); }
    bool subviewsCanOverlap() const override { return true; }
    void setHighlighted(bool highlight) override;
    void setLayout(Poincare::Layout layout);
  private:
    Escher::ExpressionView m_expressionView;
    Escher::ChevronView m_chevronView;
  };

  InteractiveCurveViewController * m_graphController;
  Escher::SelectableTableView m_selectableTableView;
};

}

#endif
