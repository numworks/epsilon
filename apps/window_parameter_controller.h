#ifndef APPS_WINDOW_PARAMETER_CONTROLLER_H
#define APPS_WINDOW_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "curve_view_window_with_cursor.h"
#include "float_parameter_controller.h"

class WindowParameterController : public FloatParameterController {
public:
  WindowParameterController(Responder * parentResponder, CurveViewWindowWithCursor * graphWindow);
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
  CurveViewWindowWithCursor * m_graphWindow;
  char m_draftTextBuffer[EditableTextMenuListCell::k_bufferLength];
  EditableTextMenuListCell m_windowCells[k_numberOfTextCell];
  SwitchMenuListCell m_yAutoCell;
};

#endif
