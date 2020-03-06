#ifndef GRAPH_LIST_DOMAIN_PARAMATER_CONTROLLER_H
#define GRAPH_LIST_DOMAIN_PARAMATER_CONTROLLER_H

#include <escher/message_table_cell_with_expression.h>
#include <ion/storage.h>
#include "../../shared/continuous_function.h"
#include "../../shared/expiring_pointer.h"
#include "../../shared/float_parameter_controller.h"

namespace Graph {

class DomainParameterController : public Shared::FloatParameterController<float> {
public:
  DomainParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);

  // ViewController
  const char * title() override;
  TELEMETRY_ID("DomainParameter");

  // ListViewDataSource
  int numberOfRows() const override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  void setRecord(Ion::Storage::Record record) { m_record = record; }
private:
  constexpr static int k_totalNumberOfCell = 2;
  int reusableParameterCellCount(int type) override;
  HighlightCell * reusableParameterCell(int index, int type) override;
  bool handleEvent(Ion::Events::Event event) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;
  float parameterAtIndex(int index) override;
  void buttonAction() override;
  InfinityTolerance infinityAllowanceForRow(int row) const override;
  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  MessageTableCellWithEditableText m_domainCells[k_totalNumberOfCell];
  Ion::Storage::Record m_record;
};

}

#endif
