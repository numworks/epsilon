#ifndef CALCULATION_ADDITIONAL_OUTPUTS_SIMPLE_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_SIMPLE_LIST_CONTROLLER_H

#include <escher.h>
#include <poincare/expression.h>
#include <apps/i18n.h>

namespace Calculation {

class SimpleListController : public StackViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  SimpleListController(Responder * parentResponder);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  //ListViewDataSource
  int reusableCellCount(int type) override;
  HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override { return 0; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  // IllustratedListController
  void setExpression(Poincare::Expression e) { m_expression = e; }

protected:
  Poincare::Expression m_expression;
private:
  virtual Poincare::Layout layoutAtIndex(int index) = 0;
  virtual I18n::Message messageAtIndex(int index) = 0;
  class ListController : public ViewController {
  public:
    ListController(SimpleListController * dataSource);
    const char * title() override { return I18n::translate(I18n::Message::AdditionalResults); }
    View * view() override { return &m_selectableTableView; }
    SelectableTableView * selectableTableView() { return &m_selectableTableView; }
  private:
    SelectableTableView m_selectableTableView;
  };
  constexpr static int k_maxNumberOfCells = 4;
  ListController m_listController;
  // Cells
  ExpressionTableCellWithPointer m_cells[k_maxNumberOfCells];
};

}

#endif

