#ifndef PROBABILITY_PARAMETERS_CONTROLLER_H
#define PROBABILITY_PARAMETERS_CONTROLLER_H

#include <escher.h>
#include "../float_parameter_controller.h"
#include "law.h"
#include "calculation_controller.h"

namespace Probability {

class ParametersController : public FloatParameterController {
public:
  ParametersController(Responder * parentResponder, Law * law);
  ExpressionTextFieldDelegate * textFieldDelegate() override;
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  StackViewController * stackController();
  CalculationController * calculationController();
  int numberOfRows() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  void updateTitle();
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, SelectableTableView * selectableTableView);
    Button * button();
    PointerTextView * parameterDefinitionAtIndex(int index);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void layoutSubviews() override;
  private:
    constexpr static KDCoordinate k_buttonHeight = 40;
    constexpr static KDCoordinate k_textHeight = 30;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    Button m_nextButton;
    PointerTextView m_firstParameterDefinition;
    PointerTextView m_secondParameterDefinition;
    SelectableTableView * m_selectableTableView;
  };
  constexpr static int k_maxNumberOfCells = 2;
  EditableTextMenuListCell m_menuListCell[k_maxNumberOfCells];
  ContentView m_contentView;
  Law * m_law;
  bool m_buttonSelected;
  CalculationController m_calculationController;
  constexpr static int k_maxNumberOfTitleCharacters = 30;
  char m_titleBuffer[k_maxNumberOfTitleCharacters];
};

}

#endif
