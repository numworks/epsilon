#include "math_toolbox.h"
#include <assert.h>
#include <string.h>

/* TODO: find a shorter way to initiate tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at I18n::Message::Default. */

const ToolboxNode calculChildren[4] = {ToolboxNode(I18n::Message::DiffCommand, I18n::Message::DerivateNumber), ToolboxNode(I18n::Message::IntCommand, I18n::Message::Integral), ToolboxNode(I18n::Message::SumCommand, I18n::Message::Sum), ToolboxNode(I18n::Message::ProductCommand, I18n::Message::Product)};
const ToolboxNode complexChildren[5] = {ToolboxNode(I18n::Message::AbsCommand, I18n::Message::ComplexAbsoluteValue), ToolboxNode(I18n::Message::ArgCommand, I18n::Message::Agument), ToolboxNode(I18n::Message::ReCommand, I18n::Message::ReelPart), ToolboxNode(I18n::Message::ImCommand, I18n::Message::ImaginaryPart), ToolboxNode(I18n::Message::ConjCommand, I18n::Message::Conjugate)};
const ToolboxNode probabilityChildren[2] = {ToolboxNode(I18n::Message::BinomialCommand, I18n::Message::Combination), ToolboxNode(I18n::Message::PermuteCommand, I18n::Message::Permutation)};
const ToolboxNode arithmeticChildren[4] = {ToolboxNode(I18n::Message::GcdCommand, I18n::Message::GreatCommonDivisor),ToolboxNode(I18n::Message::LcmCommand, I18n::Message::LeastCommonMultiple), ToolboxNode(I18n::Message::RemCommand, I18n::Message::Remainder), ToolboxNode(I18n::Message::QuoCommand, I18n::Message::Quotient)};
const ToolboxNode matricesChildren[5] = {ToolboxNode(I18n::Message::InverseCommand, I18n::Message::Inverse), ToolboxNode(I18n::Message::DeterminantCommand, I18n::Message::Determinant), ToolboxNode(I18n::Message::TransposeCommand, I18n::Message::Transpose), ToolboxNode(I18n::Message::TraceCommand, I18n::Message::Trace), ToolboxNode(I18n::Message::DimensionCommand, I18n::Message::Dimension)};
const ToolboxNode listesChildren[5] = {ToolboxNode(I18n::Message::SortCommand, I18n::Message::Sort), ToolboxNode(I18n::Message::InvSortCommand, I18n::Message::InvSort), ToolboxNode(I18n::Message::MaxCommand, I18n::Message::Maximum), ToolboxNode(I18n::Message::MinCommand, I18n::Message::Minimum), ToolboxNode(I18n::Message::DimensionCommand, I18n::Message::Dimension)};
const ToolboxNode approximationChildren[4] = {ToolboxNode(I18n::Message::FloorCommand, I18n::Message::Floor), ToolboxNode(I18n::Message::FracCommand, I18n::Message::FracPart), ToolboxNode(I18n::Message::CeilCommand, I18n::Message::Ceiling), ToolboxNode(I18n::Message::RoundCommand, I18n::Message::Rounding)};
const ToolboxNode trigonometryChildren[6] = {ToolboxNode(I18n::Message::CoshCommand, I18n::Message::Cosh), ToolboxNode(I18n::Message::SinhCommand, I18n::Message::Sinh), ToolboxNode(I18n::Message::TanhCommand, I18n::Message::Tanh), ToolboxNode(I18n::Message::AcoshCommand, I18n::Message::Acosh), ToolboxNode(I18n::Message::AsinhCommand, I18n::Message::Asinh), ToolboxNode(I18n::Message::AtanhCommand, I18n::Message::Atanh)};
const ToolboxNode predictionChildren[3] = {ToolboxNode(I18n::Message::Prediction95Command, I18n::Message::Prediction95), ToolboxNode(I18n::Message::PredictionCommand, I18n::Message::Prediction), ToolboxNode(I18n::Message::ConfidenceCommand, I18n::Message::Confidence)};

const ToolboxNode menu[12] = {ToolboxNode(I18n::Message::AbsCommand, I18n::Message::AbsoluteValue), ToolboxNode(I18n::Message::RootCommand, I18n::Message::NthRoot), ToolboxNode(I18n::Message::LogCommand, I18n::Message::BasedLogarithm),
  ToolboxNode(I18n::Message::Calculation, I18n::Message::Default, calculChildren, 4), ToolboxNode(I18n::Message::ComplexNumber, I18n::Message::Default, complexChildren, 5),
  ToolboxNode(I18n::Message::Probability, I18n::Message::Default, probabilityChildren, 2), ToolboxNode(I18n::Message::Arithmetic, I18n::Message::Default, arithmeticChildren, 4),
  ToolboxNode(I18n::Message::Matrices,  I18n::Message::Default, matricesChildren, 5), ToolboxNode(I18n::Message::Lists, I18n::Message::Default, listesChildren, 5),
  ToolboxNode(I18n::Message::Approximation, I18n::Message::Default, approximationChildren, 4), ToolboxNode(I18n::Message::HyperbolicTrigonometry, I18n::Message::Default, trigonometryChildren, 6),
  ToolboxNode(I18n::Message::Fluctuation, I18n::Message::Default, predictionChildren, 3)};
const ToolboxNode toolboxModel = ToolboxNode(I18n::Message::Toolbox, I18n::Message::Default, menu, 12);

/* State */

MathToolbox::Stack::State::State(int selectedRow, KDCoordinate verticalScroll) :
  m_selectedRow(selectedRow),
  m_verticalScroll(verticalScroll)
{
}

int MathToolbox::Stack::State::selectedRow() {
  return m_selectedRow;
}

KDCoordinate MathToolbox::Stack::State::verticalScroll() {
  return m_verticalScroll;
}

bool MathToolbox::Stack::State::isNull(){
  if (m_selectedRow == -1) {
    return true;
  }
  return false;
}

/* Stack */

void MathToolbox::Stack::push(int selectedRow, KDCoordinate verticalScroll) {
  int i = 0;
  while (!m_statesStack[i].isNull() && i < k_maxModelTreeDepth) {
    i++;
  }
  assert(m_statesStack[i].isNull());
  m_statesStack[i] = State(selectedRow, verticalScroll);
}

MathToolbox::Stack::State * MathToolbox::Stack::stateAtIndex(int index) {
  return &m_statesStack[index];
}

int MathToolbox::Stack::depth() {
  int depth = 0;
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    depth += (!m_statesStack[i].isNull());
  }
  return depth;
}

void MathToolbox::Stack::pop() {
  int stackDepth = depth();
  if (stackDepth == 0) {
    return;
  }
  m_statesStack[stackDepth-1] = State();
}

void MathToolbox::Stack::resetStack() {
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    m_statesStack[i] = State();
  }
}

/* List Controller */

MathToolbox::ListController::ListController(Responder * parentResponder, SelectableTableView * tableView) :
  ViewController(parentResponder),
  m_selectableTableView(tableView),
  m_firstSelectedRow(0)
{
}

View * MathToolbox::ListController::view() {
  return m_selectableTableView;
}

const char * MathToolbox::ListController::title() {
  return I18n::translate(toolboxModel.label());
}

void MathToolbox::ListController::didBecomeFirstResponder() {
  m_selectableTableView->reloadData();
  m_selectableTableView->selectCellAtLocation(0, m_firstSelectedRow);
  app()->setFirstResponder(m_selectableTableView);
}

void MathToolbox::ListController::setFirstSelectedRow(int firstSelectedRow) {
  m_firstSelectedRow = firstSelectedRow;
}

/* MathToolbox */

MathToolbox::MathToolbox() :
  Toolbox(nullptr, &m_listController),
  m_selectableTableView(SelectableTableView(&m_listController, this, 1, 0, 0, 0, 0, nullptr, false)),
  m_listController(this, &m_selectableTableView),
  m_nodeModel(nullptr)
{
}

void MathToolbox::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_listController);
}

bool MathToolbox::handleEvent(Ion::Events::Event event) {
  return handleEventForRow(event, m_selectableTableView.selectedRow());
}

int MathToolbox::numberOfRows() {
  if (m_nodeModel == nullptr) {
    m_nodeModel = (ToolboxNode *)rootModel();
  }
  return m_nodeModel->numberOfChildren();
}

HighlightCell * MathToolbox::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  if (type == 0) {
    return &m_leafCells[index];
  }
  return &m_nodeCells[index];
}

int MathToolbox::reusableCellCount(int type) {
  return k_maxNumberOfDisplayedRows;
}

void MathToolbox::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ToolboxNode * node = (ToolboxNode *)m_nodeModel->children(index);
  if (node->numberOfChildren() == 0) {
    MessageTableCellWithMessage * myCell = (MessageTableCellWithMessage *)cell;
    myCell->setMessage(node->label());
    myCell->setAccessoryMessage(node->text());
    myCell->setAccessoryTextColor(Palette::GreyDark);
    return;
  }
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(node->label());
}

KDCoordinate MathToolbox::rowHeight(int j) {
  if (typeAtLocation(0, j) == 0) {
    return k_leafRowHeight;
  }
  return k_nodeRowHeight;
}

KDCoordinate MathToolbox::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int MathToolbox::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

int MathToolbox::typeAtLocation(int i, int j) {
  Node * node = (Node *)m_nodeModel->children(j);
  if (node->numberOfChildren() == 0) {
    return 0;
  }
  return 1;
}

void MathToolbox::viewWillAppear() {
  Toolbox::viewWillAppear();
  m_nodeModel = (ToolboxNode *)rootModel();
  m_selectableTableView.reloadData();
  m_stack.resetStack();
  m_listController.setFirstSelectedRow(0);
}

void MathToolbox::viewWillDisappear() {
  m_selectableTableView.deselectTable();
}

int MathToolbox::stackDepth() {
  return m_stack.depth();
}

TextField * MathToolbox::sender() {
  return (TextField *)Toolbox::sender();
}

bool MathToolbox::handleEventForRow(Ion::Events::Event event, int selectedRow) {
  if (event == Ion::Events::Back) {
    return returnToPreviousMenu();
  }
  if (event == Ion::Events::OK) {
    ToolboxNode * selectedNode = (ToolboxNode *)m_nodeModel->children(selectedRow);
    if (selectedNode->numberOfChildren() == 0) {
      return selectLeaf(selectedNode);
    }
    return selectSubMenu(selectedNode);
  }
  return false;
}

const ToolboxNode * MathToolbox::rootModel() {
  return &toolboxModel;
}

bool MathToolbox::selectLeaf(ToolboxNode * selectedNode){
  m_selectableTableView.deselectTable();
  ToolboxNode * node = selectedNode;
  const char * editedText = I18n::translate(node->label());
  if (!sender()->isEditing()) {
    sender()->setEditing(true);
  }
  sender()->insertTextAtLocation(editedText, sender()->cursorLocation());
  int cursorDelta = 0;
  int editedTextLength = strlen(editedText);
  for (int i = 0; i < editedTextLength; i++) {
    if (editedText[i] == '(') {
      cursorDelta =  i + 1;
      break;
    }
  }
  sender()->setCursorLocation(sender()->cursorLocation()+cursorDelta);
  app()->dismissModalViewController();
  return true;
}

bool MathToolbox::returnToPreviousMenu() {
  m_selectableTableView.deselectTable();
  int depth = m_stack.depth();
  if (depth == 0) {
    app()->dismissModalViewController();
    return true;
  }
  int index = 0;
  ToolboxNode * parentNode = (ToolboxNode *)rootModel();
  Stack::State * previousState = m_stack.stateAtIndex(index++);;
  while (depth-- > 1) {
    parentNode = (ToolboxNode *)parentNode->children(previousState->selectedRow());
    previousState = m_stack.stateAtIndex(index++);
  }
  m_selectableTableView.deselectTable();
  m_nodeModel = parentNode;
  m_listController.setFirstSelectedRow(previousState->selectedRow());
  KDPoint scroll = m_selectableTableView.contentOffset();
  m_selectableTableView.setContentOffset(KDPoint(scroll.x(), previousState->verticalScroll()));
  m_stack.pop();
  app()->setFirstResponder(&m_listController);
  return true;
}

bool MathToolbox::selectSubMenu(ToolboxNode * selectedNode) {
  m_stack.push(m_selectableTableView.selectedRow(),  m_selectableTableView.contentOffset().y());
  m_selectableTableView.deselectTable();
  m_nodeModel = selectedNode;
  m_listController.setFirstSelectedRow(0);
  app()->setFirstResponder(&m_listController);
  return true;
}
