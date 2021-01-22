#ifndef SHARED_FLOAT_PARAMETER_CONTROLLER_H
#define SHARED_FLOAT_PARAMETER_CONTROLLER_H

#include "parameter_text_field_delegate.h"
#include "button_with_separator.h"
#include <escher/simple_list_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/stack_view_controller.h>

namespace Shared {

/* This controller edits float parameter of any model (given through
 * parameterAtIndex and setParameterAtIndex). */

template<typename T>
class FloatParameterController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource, public ParameterTextFieldDelegate {
public:
  FloatParameterController(Responder * parentResponder);
  Escher::View * view() override { return &m_selectableTableView; }
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  bool handleEvent(Ion::Events::Event event) override;

  int typeAtIndex(int index) override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  KDCoordinate cellWidth() override {
    if (m_selectableTableView.columnWidth(0) <= 0) {
      return 200; // TODO
    }
    return m_selectableTableView.columnWidth(0);
  }
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  bool textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;
protected:
  enum class InfinityTolerance {
    None,
    PlusInfinity,
    MinusInfinity
  };
  int activeCell();
  Escher::StackViewController * stackController();
  virtual T parameterAtIndex(int index) = 0;
  virtual void buttonAction();
  Escher::SelectableTableView m_selectableTableView;
  ButtonWithSeparator m_okButton;
private:
  constexpr static int k_buttonMargin = 6;
  virtual InfinityTolerance infinityAllowanceForRow(int row) const { return InfinityTolerance::None; }
  virtual int reusableParameterCellCount(int type) = 0;
  virtual Escher::HighlightCell * reusableParameterCell(int index, int type) = 0;
  virtual bool setParameterAtIndex(int parameterIndex, T f) = 0;
};

}

#endif
