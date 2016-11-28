#include "axis_parameter_controller.h"
#include <assert.h>

namespace Graph {

AxisParameterController::AxisParameterController(Responder * parentResponder, AxisInterval * axisInterval) :
  FloatParameterController(parentResponder),
  m_axisInterval(axisInterval),
  m_yAutoCell(SwitchMenuListCell((char*)"Y auto"))
{
  m_axisCells[0].setText("Xmin");
  m_axisCells[1].setText("Xmax");
  m_axisCells[2].setText("Ymin");
  m_axisCells[3].setText("Ymax");

}

const char * AxisParameterController::title() const {
  return "Axes";
}

void AxisParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  if (index == 2) {
    SwitchView * switchView = (SwitchView *)m_yAutoCell.accessoryView();
    switchView->setState(m_axisInterval->yAuto());
    return;
  }
  if (index == 3 || index == 4) {
    if (m_axisInterval->yAuto()) {
      m_axisCells[index-1].setTextColor(Palette::DesactiveTextColor);
    } else {
      m_axisCells[index-1].setTextColor(KDColorBlack);
    }
  }
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool AxisParameterController::handleEvent(Ion::Events::Event event) {
  if (activeCell() == 2) {
    if (event == Ion::Events::OK) {
      m_axisInterval->setYAuto(!m_axisInterval->yAuto());
      m_selectableTableView.reloadData();
      return true;
    }
    return false;
  }
  if (m_axisInterval->yAuto() && (activeCell() == 3 || activeCell() == 4)) {
    return false;
  }
  return FloatParameterController::handleEvent(event);
}

float AxisParameterController::parameterAtIndex(int index) {
  switch (index) {
    case 0:
      return m_axisInterval->xMin();
    case 1:
      return m_axisInterval->xMax();
    case 3:
      return m_axisInterval->yMin();
    case 4:
      return m_axisInterval->yMax();
    default:
      assert(false);
      return 0.0f;
  }
}

void AxisParameterController::setParameterAtIndex(int parameterIndex, float f) {
  switch(parameterIndex) {
    case 0:
      m_axisInterval->setXMin(f);
      break;
    case 1:
      m_axisInterval->setXMax(f);
      break;
    case 3:
      m_axisInterval->setYMin(f);
      break;
    case 4:
      m_axisInterval->setYMax(f);
      break;
    default:
      assert(false);
  }
}

int AxisParameterController::numberOfRows() {
  return k_numberOfTextCell+1;
};

TableViewCell * AxisParameterController::reusableCell(int index) {
  if (index == 2) {
    return &m_yAutoCell;
  }
  int cellIndex = index > 2 ? index - 1 : index;
  assert(cellIndex >= 0);
  assert(cellIndex < k_numberOfTextCell);
  return &m_axisCells[cellIndex];
}

int AxisParameterController::reusableCellCount() {
  return k_numberOfTextCell+1;
}

}
