#ifndef SHARED_FLOAT_PARAMETER_CONTROLLER_H
#define SHARED_FLOAT_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "parameter_text_field_delegate.h"
#include "button_with_separator.h"

namespace Shared {

/* This controller edits float parameter of any model (given through
 * parameterAtIndex and setParameterAtIndex). */

class FloatParameterController : public DynamicViewController, public ListViewDataSource, public SelectableTableViewDataSource, public ParameterTextFieldDelegate {
public:
  FloatParameterController(Responder * parentResponder);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  bool handleEvent(Ion::Events::Event event) override;

  int typeAtLocation(int i, int j) override;
  int reusableCellCount(int type) override;
  HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
protected:
  int activeCell();
  StackViewController * stackController();
  virtual double parameterAtIndex(int index) = 0;
  virtual SelectableTableView * selectableTableView();
  View * loadView() override;
  void unloadView(View * view) override;
private:
  constexpr static int k_buttonMargin = 6;
  virtual void buttonAction();
  virtual I18n::Message okButtonText();
  virtual int reusableParameterCellCount(int type) = 0;
  virtual HighlightCell * reusableParameterCell(int index, int type) = 0;
  TextFieldDelegateApp * textFieldDelegateApp() override;
  virtual bool setParameterAtIndex(int parameterIndex, double f) = 0;
  ButtonWithSeparator * m_okButton;
};

}

#endif
