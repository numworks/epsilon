#include "variable_box_controller.h"
#include "variable_box_node.h"

const VariableBoxNode numberChildren[10] = {VariableBoxNode("A"), VariableBoxNode("B"), VariableBoxNode("C"),
 VariableBoxNode("D"), VariableBoxNode("E"), VariableBoxNode("F"), VariableBoxNode("G"), VariableBoxNode("H"),
 VariableBoxNode("I"), VariableBoxNode("J")};
const VariableBoxNode listChildren[10] = {VariableBoxNode("L1"), VariableBoxNode("L2"), VariableBoxNode("L3"),
 VariableBoxNode("L4"), VariableBoxNode("L5"), VariableBoxNode("L6"), VariableBoxNode("L7"), VariableBoxNode("L8"),
 VariableBoxNode("L9"), VariableBoxNode("L10")};
const VariableBoxNode matriceChildren[10] = {VariableBoxNode("M1"), VariableBoxNode("M2"), VariableBoxNode("M3"),
 VariableBoxNode("M4"), VariableBoxNode("M5"), VariableBoxNode("M6"), VariableBoxNode("M7"), VariableBoxNode("M8"),
 VariableBoxNode("M9"), VariableBoxNode("M10")};
const VariableBoxNode menu[3] = {VariableBoxNode("Nombres", numberChildren, 10), VariableBoxNode("Listes", listChildren, 10),
  VariableBoxNode("Matrices", matriceChildren, 10)};
const VariableBoxNode variableBoxModel = VariableBoxNode("Variables", menu, 3);


const char * VariableBoxController::title() const {
  return "VariableBoxController";
}

TableViewCell * VariableBoxController::leafCellAtIndex(int index) {
  return &m_leafCells[index];
}

TableViewCell * VariableBoxController::nodeCellAtIndex(int index) {
  return &m_nodeCells[index];
}

void VariableBoxController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  MenuListCell * myCell = (MenuListCell *)cell;
  myCell->setText(m_listViewController.nodeModel()->children(index)->label());
}

Node * VariableBoxController::nodeModel() {
  return (Node *)&variableBoxModel;
}

bool VariableBoxController::selectLeaf(Node * selectedNode){
  m_listViewController.deselectTable();
  const char * editedText = selectedNode->label();
  m_textFieldCaller->appendText(editedText);
  app()->dismissModalViewController();
  return true;
}
