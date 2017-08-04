#ifndef PROBABILITY_PARAMETERS_CONTROLLER_H
#define PROBABILITY_PARAMETERS_CONTROLLER_H

#include <escher.h>
#include "../shared/float_parameter_controller.h"
#include "law/law.h"
#include "calculation_controller.h"

namespace Probability {

class ParametersController : public Shared::FloatParameterController {
public:
  ParametersController(Responder * parentResponder, Law * m_law, CalculationController * calculationController);
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void reinitCalculation();
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
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
    ContentView(Responder * parentResponder, SelectableTableView * selectableTableView);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    MessageTextView * parameterDefinitionAtIndex(int index);
    void layoutSubviews() override;
    void setNumberOfParameters(int numberOfParameters);
  private:
    constexpr static KDCoordinate k_textMargin = 5;
    constexpr static KDCoordinate k_titleMargin = 5;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    int m_numberOfParameters;
    MessageTextView m_titleView;
    MessageTextView m_firstParameterDefinition;
    MessageTextView m_secondParameterDefinition;
    SelectableTableView * m_selectableTableView;
  };
  SelectableTableView * selectableTableView() override;
  ContentView * contentView();
  SelectableTableView * m_selectableTableView;
  constexpr static int k_maxNumberOfCells = 2;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
  MessageTableCellWithEditableText * m_menuListCell[k_maxNumberOfCells];
  Law * m_law;
  CalculationController * m_calculationController;
};

}

#endif
