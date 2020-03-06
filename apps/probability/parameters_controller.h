#ifndef PROBABILITY_PARAMETERS_CONTROLLER_H
#define PROBABILITY_PARAMETERS_CONTROLLER_H

#include <escher.h>
#include "../shared/float_parameter_controller.h"
#include "distribution/distribution.h"
#include "calculation_controller.h"

namespace Probability {

class ParametersController : public Shared::FloatParameterController<float> {
public:
  ParametersController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Distribution * m_distribution, CalculationController * calculationController);
  const char * title() override;
  View * view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  void reinitCalculation();
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  TELEMETRY_ID("Parameters");
  int numberOfRows() const override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  void buttonAction() override;
  float parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  class ContentView : public View {
  public:
    ContentView(SelectableTableView * selectableTableView);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    MessageTextView * parameterDefinitionAtIndex(int index);
    void layoutSubviews(bool force = false) override;
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
  constexpr static int k_maxNumberOfCells = 2;
  ContentView m_contentView;
  MessageTableCellWithEditableText m_menuListCell[k_maxNumberOfCells];
  Distribution * m_distribution;
  CalculationController * m_calculationController;
};

}

#endif
