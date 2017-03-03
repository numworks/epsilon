#ifndef SETTINGS_SUB_CONTROLLER_H
#define SETTINGS_SUB_CONTROLLER_H

#include <escher.h>
#include "settings_node.h"

namespace Settings {

class SubController : public ViewController, public SimpleListViewDataSource {
public:
  SubController(Responder * parentResponder);
  ~SubController();
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void setNodeModel(const Node * nodeModel, int preferenceIndex);
  void viewWillAppear() override;
private:
  StackViewController * stackController() const;
  void setPreferenceAtIndexWithValueIndex(int preferenceIndex, int valueIndex);
  int valueIndexAtPreferenceIndex(int preferenceIndex);
  constexpr static int k_totalNumberOfCell = 3;
  PointerTableCell m_cells[k_totalNumberOfCell];
  ExpressionTableCell m_complexFormatCells[2];
  Poincare::ExpressionLayout * m_complexFormatLayout[2];
  SelectableTableView m_selectableTableView;
  Node * m_nodeModel;
  int m_preferenceIndex;
};

}

#endif
