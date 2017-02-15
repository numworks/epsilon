#ifndef SHARED_FLOAT_PARAMETER_CONTROLLER_H
#define SHARED_FLOAT_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "text_field_delegate.h"

namespace Shared {

/* This controller edits float parameter of any model (given through
 * parameterAtIndex and setParameterAtIndex). */

class FloatParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDelegate, public TextFieldDelegate {
public:
  FloatParameterController(Responder * parentResponder);
  View * view() override;
  void didBecomeFirstResponder() override;
  KDCoordinate cellHeight() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
protected:
  int activeCell();
  SelectableTableView m_selectableTableView;
private:
  TextFieldDelegateApp * textFieldDelegateApp() override;
  virtual float parameterAtIndex(int index) = 0;
  virtual void setParameterAtIndex(int parameterIndex, float f) = 0;
};

}

#endif
