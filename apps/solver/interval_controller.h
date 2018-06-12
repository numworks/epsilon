#ifndef SOLVER_INTERVAL_CONTROLLER_H
#define SOLVER_INTERVAL_CONTROLLER_H

#include <escher.h>
#include "equation_store.h"
#include "../shared/float_parameter_controller.h"

namespace Solver {

class IntervalController : public Shared::FloatParameterController {
public:
  IntervalController(Responder * parentResponder, EquationStore * equationStore);
  const char * title() override;
  int numberOfRows() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  void buttonAction() override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  I18n::Message okButtonText() override;
  View * loadView() override;
  void unloadView(View * view) override;
  class ContentView : public View {
  public:
    ContentView(SelectableTableView * selectableTableView);
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    constexpr static KDCoordinate k_topMargin = 50;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    MessageTextView m_instructions0;
    MessageTextView m_instructions1;
    SelectableTableView * m_selectableTableView;
  };
  SelectableTableView * selectableTableView() override;
  SelectableTableView * m_selectableTableView;
  constexpr static int k_maxNumberOfCells = 2;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
  MessageTableCellWithEditableText * m_intervalCell[k_maxNumberOfCells];
  EquationStore * m_equationStore;
};

}

#endif
