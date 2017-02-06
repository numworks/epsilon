#ifndef GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "goto_parameter_controller.h"
#include "banner_view.h"
#include "../function.h"
#include "../../shared/curve_view_cursor.h"
#include "../../shared/interactive_curve_view_range.h"

namespace Graph {

class CurveParameterController : public ViewController, public SimpleListViewDataSource {
public:
  CurveParameterController(Shared::InteractiveCurveViewRange * graphRange, BannerView * bannerView, Shared::CurveViewCursor * cursor);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  void setFunction(Function * function);
private:
  BannerView * m_bannerView;
  Function * m_function;
  constexpr static int k_totalNumberOfCells = 3;
  ChevronMenuListCell m_calculationCell;
  ChevronMenuListCell m_goToCell;
  SwitchMenuListCell m_derivativeCell;
  SelectableTableView m_selectableTableView;
  GoToParameterController m_goToParameterController;
};

}

#endif
