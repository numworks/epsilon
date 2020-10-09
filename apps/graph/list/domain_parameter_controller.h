#ifndef GRAPH_LIST_DOMAIN_PARAMATER_CONTROLLER_H
#define GRAPH_LIST_DOMAIN_PARAMATER_CONTROLLER_H

#include <escher/message_table_cell_with_expression.h>
#include <ion/storage.h>
#include "../../shared/continuous_function.h"
#include "../../shared/expiring_pointer.h"
#include "../../shared/float_parameter_controller.h"
#include "../../shared/range_1D.h"
#include "../../shared/discard_pop_up_controller.h"

namespace Graph {

class DomainParameterController : public Shared::FloatParameterController<float> {
public:
  DomainParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);

  // ViewController
  const char * title() override { return I18n::translate(I18n::Message::FunctionDomain); }
  TELEMETRY_ID("DomainParameter");

  // ListViewDataSource
  int numberOfRows() const override { return k_totalNumberOfCell+1; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  void setRecord(Ion::Storage::Record record) { m_record = record; }
private:
  constexpr static int k_totalNumberOfCell = 2;
  void viewWillAppear() override;
  int reusableParameterCellCount(int type) override { return k_totalNumberOfCell; }
  HighlightCell * reusableParameterCell(int index, int type) override;
  bool handleEvent(Ion::Events::Event event) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;
  float parameterAtIndex(int index) override;
  void buttonAction() override;
  InfinityTolerance infinityAllowanceForRow(int row) const override;
  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  // Applies temporary parameters to function.
  void confirmParameters();
  // Extracts parameters from function, setting m_tempDomain parameters.
  void extractParameters();
  // Return true if temporary parameters and function parameters are equal.
  bool equalTempParameters();
  MessageTableCellWithEditableText m_domainCells[k_totalNumberOfCell];
  Ion::Storage::Record m_record;
  Shared::Range1D m_tempDomain;
  Shared::DiscardPopUpController m_confirmPopUpController;
};

}

#endif
