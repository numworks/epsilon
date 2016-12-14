#ifndef PROBABILITY_LAW_CONTROLLER_H
#define PROBABILITY_LAW_CONTROLLER_H

#include <escher.h>
#include "cell.h"
#include "law/law.h"
#include "parameters_controller.h"
#include "evaluate_context.h"

namespace Probability {

class LawController : public ViewController, public SimpleListViewDataSource {
public:
  LawController(Responder * parentResponder, EvaluateContext * EvaluateContext);
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  int numberOfRows() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  void setLawAccordingToIndex(int index);
  constexpr static int k_totalNumberOfModels = 5;
  // !!! CAUTION: The order here is important
  // The cells should be initialized *before* the listview!
  Cell m_cells[k_totalNumberOfModels];
  SelectableTableView m_selectableTableView;
  const char ** m_messages;
  Law * m_law;
  EvaluateContext * m_evaluateContext;
  ParametersController m_parametersController;

};

}

#endif
