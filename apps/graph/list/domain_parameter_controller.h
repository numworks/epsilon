#ifndef GRAPH_LIST_DOMAIN_PARAMATER_CONTROLLER_H
#define GRAPH_LIST_DOMAIN_PARAMATER_CONTROLLER_H

#include <escher/message_table_cell_with_expression.h>
#include <ion/storage.h>
#include "../../shared/cartesian_function.h"
#include "../../shared/expiring_pointer.h"
#include "../../shared/float_parameter_controller.h"

namespace Graph {

class DomainParameterController : public Shared::FloatParameterController<double> {
public:
  DomainParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);

  // ViewController
  const char * title() override;

  // ListViewDataSource
  int numberOfRows() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  void setRecord(Ion::Storage::Record record) { m_record = record; }
private:
  constexpr static int k_totalNumberOfCell = 2;
  int reusableParameterCellCount(int type) override;
  HighlightCell * reusableParameterCell(int index, int type) override;
  bool handleEvent(Ion::Events::Event event) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  double parameterAtIndex(int index) override;
  void buttonAction() override;
  Shared::ExpiringPointer<Shared::CartesianFunction> function();
  MessageTableCellWithEditableText m_domainCells[k_totalNumberOfCell];
  Ion::Storage::Record m_record;
};

}

#endif
