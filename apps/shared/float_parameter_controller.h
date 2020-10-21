#ifndef SHARED_FLOAT_PARAMETER_CONTROLLER_H
#define SHARED_FLOAT_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "parameter_text_field_delegate.h"
#include "button_with_separator.h"

namespace Shared {

/* This controller edits float parameter of any model (given through
 * parameterAtIndex and setParameterAtIndex). */

template<typename T>
class FloatParameterController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource, public ParameterTextFieldDelegate {
public:
  FloatParameterController(Responder * parentResponder);
  View * view() override { return &m_selectableTableView; }
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
  enum class InfinityTolerance {
    None,
    PlusInfinity,
    MinusInfinity
  };
  int activeCell();
  StackViewController * stackController();
  virtual T parameterAtIndex(int index) = 0;
  virtual void buttonAction();
  SelectableTableView m_selectableTableView;
  ButtonWithSeparator m_okButton;
private:
  constexpr static int k_buttonMargin = 6;
  virtual InfinityTolerance infinityAllowanceForRow(int row) const { return InfinityTolerance::None; }
  virtual int reusableParameterCellCount(int type) = 0;
  virtual HighlightCell * reusableParameterCell(int index, int type) = 0;
  virtual bool setParameterAtIndex(int parameterIndex, T f) = 0;
};

}

#endif
