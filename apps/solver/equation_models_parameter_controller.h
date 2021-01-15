#ifndef SOLVER_EQUATION_MODELS_PARAMETER_CONTROLLER_H
#define SOLVER_EQUATION_MODELS_PARAMETER_CONTROLLER_H

#include <escher/list_view_data_source.h>
#include <escher/expression_table_cell.h>
#include <escher/message_table_cell.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/stack_view_controller.h>
#include "equation_store.h"

namespace Solver {

class ListController;

class EquationModelsParameterController : public Escher::ViewController, public Escher::ListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  EquationModelsParameterController(Escher::Responder * parentResponder, EquationStore * equationStore, ListController * listController);
  const char * title() override;
  Escher::View * view() override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cellWidth() override {
    assert(m_selectableTableView.columnWidth(0) > 0);
    return m_selectableTableView.columnWidth(0);
  }
  // KDCoordinate cumulatedHeightFromIndex(int j) override;
  // int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
private:
  constexpr static int k_numberOfModels = 6;
  static constexpr const char * k_models[k_numberOfModels] = {
    "", "x+y=0", "x^2+x+1=0", "x+y+z=0", "x^3+x^2+x+1=0", "x+y+z+t=0"
  };
  Escher::StackViewController * stackController() const;
  constexpr static int k_numberOfExpressionCells = k_numberOfModels-1;
  Escher::MessageTableCell m_emptyModelCell;
  Escher::ExpressionTableCell m_modelCells[k_numberOfExpressionCells];
  Poincare::Layout m_layouts[k_numberOfExpressionCells];
  Escher::SelectableTableView m_selectableTableView;
  EquationStore * m_equationStore;
  ListController * m_listController;
};

}

#endif
