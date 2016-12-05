#ifndef APPS_FLOAT_PARAMETER_CONTROLLER_H
#define APPS_FLOAT_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "expression_text_field_delegate.h"

/* This controller edits float parameter of any model (given through
 * parameterAtIndex and setParameterAtIndex). */

class FloatParameterController : public ViewController, public SimpleListViewDataSource {
public:
  FloatParameterController(Responder * parentResponder);
  int activeCell();
  void editParameter(const char * initialText = nullptr);
  virtual ExpressionTextFieldDelegate * textFieldDelegate() = 0;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  KDCoordinate cellHeight() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
protected:
  SelectableTableView m_selectableTableView;
private:
  virtual float parameterAtIndex(int index) = 0;
  virtual void setParameterAtIndex(int parameterIndex, float f) = 0;

};

#endif
