#ifndef SEQUENCE_LIST_PARAM_CONTROLLER_H
#define SEQUENCE_LIST_PARAM_CONTROLLER_H

#include "../../shared/list_parameter_controller.h"
#include "../sequence.h"
#include "../sequence_store.h"
#include "type_parameter_controller.h"

namespace Sequence {

class ListParameterController : public Shared::ListParameterController {
public:
  ListParameterController(Responder * parentResponder, SequenceStore * sequenceStore);
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setFunction(Shared::Function * function) override;
  int numberOfRows() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCell = 4;
  ChevronExpressionMenuListCell m_typeCell;
  TypeParameterController m_typeParameterController;
  Sequence * m_sequence;
};

}

#endif
