#ifndef SHARED_FLOAT_PARAMETER_CONTROLLER_H
#define SHARED_FLOAT_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "text_field_delegate.h"

namespace Shared {

/* This controller edits float parameter of any model (given through
 * parameterAtIndex and setParameterAtIndex). */

class FloatParameterController : public ViewController, public ListViewDataSource, public SelectableTableViewDelegate, public TextFieldDelegate {
public:
  FloatParameterController(Responder * parentResponder, const char * okButtonText = "Valider");
  View * view() override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  int typeAtLocation(int i, int j) override;
  int reusableCellCount(int type) override;
  HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
protected:
  int activeCell();
  StackViewController * stackController();
  SelectableTableView m_selectableTableView;
private:
  constexpr static int k_buttonMargin = 5;
  virtual void buttonAction();
  virtual int reusableParameterCellCount(int type) = 0;
  virtual HighlightCell * reusableParameterCell(int index, int type) = 0;
  TextFieldDelegateApp * textFieldDelegateApp() override;
  virtual float previousParameterAtIndex(int index) = 0;
  virtual float parameterAtIndex(int index) = 0;
  virtual void setParameterAtIndex(int parameterIndex, float f) = 0;
  Button m_okButton;
};

}

#endif
