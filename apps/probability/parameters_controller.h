#ifndef PROBABILITY_PARAMETERS_CONTROLLER_H
#define PROBABILITY_PARAMETERS_CONTROLLER_H

#include <escher.h>
#include "../shared/float_parameter_controller.h"
#include "law/law.h"
#include "calculation_controller.h"

namespace Probability {

class ParametersController : public Shared::FloatParameterController {
public:
  ParametersController(Responder * parentResponder);
  View * view() override;
  const char * title() const override;
  void setLaw(Law * law);
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  StackViewController * stackController();
  CalculationController * calculationController();
  int numberOfRows() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, SelectableTableView * selectableTableView);
    Button * button();
    PointerTextView * parameterDefinitionAtIndex(int index);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void layoutSubviews() override;
    void setNumberOfParameters(int numberOfParameters);
  private:
    constexpr static KDCoordinate k_buttonHeight = 40;
    constexpr static KDCoordinate k_textMargin = 5;
    constexpr static KDCoordinate k_titleMargin = 5;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    int m_numberOfParameters;
    Button m_nextButton;
    PointerTextView m_titleView;
    PointerTextView m_firstParameterDefinition;
    PointerTextView m_secondParameterDefinition;
    SelectableTableView * m_selectableTableView;
  };
  constexpr static int k_maxNumberOfCells = 2;
  char m_draftTextBuffer[PointerTableCellWithEditableText::k_bufferLength];
  PointerTableCellWithEditableText m_menuListCell[k_maxNumberOfCells];
  ContentView m_contentView;
  Law * m_law;
  bool m_buttonSelected;
  CalculationController m_calculationController;
};

}

#endif
