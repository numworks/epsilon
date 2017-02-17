#include "list_parameter_controller.h"
#include "../../../poincare/src/layout/baseline_relative_layout.h"
#include "../../../poincare/src/layout/string_layout.h"

using namespace Poincare;
using namespace Shared;

namespace Sequence {

ListParameterController::ListParameterController(Responder * parentResponder, SequenceStore * sequenceStore) :
  Shared::ListParameterController(parentResponder, sequenceStore),
  m_typeCell(ChevronExpressionMenuListCell((char *)"Type de suite")),
  m_sequence(nullptr)
{
}

ListParameterController::~ListParameterController() {
  if (m_typeLayout) {
    delete m_typeLayout;
  }
}

const char * ListParameterController::title() const {
  return "Options de la suite";
}

void ListParameterController::setSequence(Sequence * sequence) {
  setFunction(sequence);
  m_sequence = sequence;
  if (m_typeLayout != nullptr) {
    delete m_typeLayout;
  }
  if (m_sequence->type() == Sequence::Type::Explicite) {
    m_typeLayout = new BaselineRelativeLayout(new StringLayout(m_sequence->name(), 1), new StringLayout("n", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_sequence->type() == Sequence::Type::SingleRecurrence) {
    m_typeLayout = new BaselineRelativeLayout(new StringLayout(m_sequence->name(), 1), new StringLayout("n+1", 3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_sequence->type() == Sequence::Type::DoubleRecurrence) {
    m_typeLayout = new BaselineRelativeLayout(new StringLayout(m_sequence->name(), 1), new StringLayout("n+2", 3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  m_typeCell.setExpression(m_typeLayout);
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  if (Shared::ListParameterController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::OK && m_selectableTableView.selectedRow() == 4) {
    return false;
  }
  return false;
}

int ListParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * ListParameterController::reusableCell(int index) {
  if (index == 3) {
    return &m_typeCell;
  }
  return Shared::ListParameterController::reusableCell(index);
}

int ListParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

}
