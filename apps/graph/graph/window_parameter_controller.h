#ifndef GRAPH_GRAPH_WINDOW_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_WINDOW_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "graph_window.h"
#include "graph_view.h"
#include "../../float_parameter_controller.h"

namespace Graph {
class WindowParameterController : public FloatParameterController {
public:
  WindowParameterController(Responder * parentResponder, GraphWindow * graphWindow, GraphView * graphView);
  ExpressionTextFieldDelegate * textFieldDelegate() override;
  const char * title() const override;
  int numberOfRows() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  constexpr static int k_numberOfTextCell = 4;
  GraphWindow * m_graphWindow;
  char m_draftTextBuffer[EditableTextMenuListCell::k_bufferLength];
  EditableTextMenuListCell m_windowCells[k_numberOfTextCell];
  SwitchMenuListCell m_yAutoCell;
  GraphView * m_graphView;
};

}

#endif
