#include "math_toolbox.h"
#include <assert.h>
#include <string.h>

/* TODO: find a shorter way to initiate tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at I18n::Message::Default. */

const ToolboxNode calculChildren[4] = {ToolboxNode(I18n::Message::DiffCommandWithArg, I18n::Message::DerivateNumber, I18n::Message::DiffCommand), ToolboxNode(I18n::Message::IntCommandWithArg, I18n::Message::Integral, I18n::Message::IntCommand), ToolboxNode(I18n::Message::SumCommandWithArg, I18n::Message::Sum, I18n::Message::SumCommand), ToolboxNode(I18n::Message::ProductCommandWithArg, I18n::Message::Product, I18n::Message::ProductCommand)};
const ToolboxNode complexChildren[5] = {ToolboxNode(I18n::Message::AbsCommandWithArg, I18n::Message::ComplexAbsoluteValue, I18n::Message::AbsCommand), ToolboxNode(I18n::Message::ArgCommandWithArg, I18n::Message::Agument, I18n::Message::ArgCommand), ToolboxNode(I18n::Message::ReCommandWithArg, I18n::Message::ReelPart, I18n::Message::ReCommand), ToolboxNode(I18n::Message::ImCommandWithArg, I18n::Message::ImaginaryPart, I18n::Message::ImCommand), ToolboxNode(I18n::Message::ConjCommandWithArg, I18n::Message::Conjugate, I18n::Message::ConjCommand)};
const ToolboxNode probabilityChildren[2] = {ToolboxNode(I18n::Message::BinomialCommandWithArg, I18n::Message::Combination, I18n::Message::BinomialCommand), ToolboxNode(I18n::Message::PermuteCommandWithArg, I18n::Message::Permutation, I18n::Message::PermuteCommand)};
const ToolboxNode arithmeticChildren[4] = {ToolboxNode(I18n::Message::GcdCommandWithArg, I18n::Message::GreatCommonDivisor, I18n::Message::GcdCommand),ToolboxNode(I18n::Message::LcmCommandWithArg, I18n::Message::LeastCommonMultiple, I18n::Message::LcmCommand), ToolboxNode(I18n::Message::RemCommandWithArg, I18n::Message::Remainder, I18n::Message::RemCommand), ToolboxNode(I18n::Message::QuoCommandWithArg, I18n::Message::Quotient, I18n::Message::QuoCommand)};
#if MATRICES_ARE_DEFINED
const ToolboxNode matricesChildren[5] = {ToolboxNode(I18n::Message::InverseCommandWithArg, I18n::Message::Inverse, I18n::Message::InverseCommand), ToolboxNode(I18n::Message::DeterminantCommandWithArg, I18n::Message::Determinant, I18n::Message::DeterminantCommand), ToolboxNode(I18n::Message::TransposeCommandWithArg, I18n::Message::Transpose, I18n::Message::TransposeCommand), ToolboxNode(I18n::Message::TraceCommandWithArg, I18n::Message::Trace, I18n::Message::TraceCommand), ToolboxNode(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension, I18n::Message::DimensionCommand)};
#endif
#if LIST_ARE_DEFINED
const ToolboxNode listesChildren[5] = {ToolboxNode(I18n::Message::SortCommandWithArg, I18n::Message::Sort, I18n::Message::SortCommand), ToolboxNode(I18n::Message::InvSortCommandWithArg, I18n::Message::InvSort, I18n::Message::InvSortCommand), ToolboxNode(I18n::Message::MaxCommandWithArg, I18n::Message::Maximum, I18n::Message::MaxCommand), ToolboxNode(I18n::Message::MinCommandWithArg, I18n::Message::Minimum, I18n::Message::MinCommand), ToolboxNode(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension, I18n::Message::DimensionCommand)};
#endif
const ToolboxNode approximationChildren[4] = {ToolboxNode(I18n::Message::FloorCommandWithArg, I18n::Message::Floor, I18n::Message::FloorCommand), ToolboxNode(I18n::Message::FracCommandWithArg, I18n::Message::FracPart, I18n::Message::FracCommand), ToolboxNode(I18n::Message::CeilCommandWithArg, I18n::Message::Ceiling, I18n::Message::CeilCommand), ToolboxNode(I18n::Message::RoundCommandWithArg, I18n::Message::Rounding, I18n::Message::RoundCommand)};
const ToolboxNode trigonometryChildren[6] = {ToolboxNode(I18n::Message::CoshCommandWithArg, I18n::Message::HyperbolicCosine, I18n::Message::CoshCommand), ToolboxNode(I18n::Message::SinhCommandWithArg, I18n::Message::HyperbolicSine, I18n::Message::SinhCommand), ToolboxNode(I18n::Message::TanhCommandWithArg, I18n::Message::HyperbolicTangent, I18n::Message::TanhCommand), ToolboxNode(I18n::Message::AcoshCommandWithArg, I18n::Message::InverseHyperbolicCosine, I18n::Message::AcoshCommand), ToolboxNode(I18n::Message::AsinhCommandWithArg, I18n::Message::InverseHyperbolicSine, I18n::Message::AsinhCommand), ToolboxNode(I18n::Message::AtanhCommandWithArg, I18n::Message::InverseHyperbolicTangent, I18n::Message::AtanhCommand)};
const ToolboxNode predictionChildren[3] = {ToolboxNode(I18n::Message::Prediction95CommandWithArg, I18n::Message::Prediction95, I18n::Message::Prediction95Command), ToolboxNode(I18n::Message::PredictionCommandWithArg, I18n::Message::Prediction, I18n::Message::PredictionCommand), ToolboxNode(I18n::Message::ConfidenceCommandWithArg, I18n::Message::Confidence, I18n::Message::ConfidenceCommand)};

#if LIST_ARE_DEFINED
const ToolboxNode menu[12] = {ToolboxNode(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue, I18n::Message::AbsCommand),
#elif MATRICES_ARE_DEFINED
const ToolboxNode menu[11] = {ToolboxNode(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue, I18n::Message::AbsCommand),
#else
const ToolboxNode menu[10] = {ToolboxNode(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue, I18n::Message::AbsCommand),
#endif
  ToolboxNode(I18n::Message::RootCommandWithArg, I18n::Message::NthRoot, I18n::Message::RootCommand),
  ToolboxNode(I18n::Message::LogCommandWithArg, I18n::Message::BasedLogarithm, I18n::Message::LogCommand),
  ToolboxNode(I18n::Message::Calculation, I18n::Message::Default, I18n::Message::Default, calculChildren, 4),
  ToolboxNode(I18n::Message::ComplexNumber, I18n::Message::Default, I18n::Message::Default, complexChildren, 5),
  ToolboxNode(I18n::Message::Probability, I18n::Message::Default, I18n::Message::Default, probabilityChildren, 2),
  ToolboxNode(I18n::Message::Arithmetic, I18n::Message::Default, I18n::Message::Default, arithmeticChildren, 4),
#if MATRICES_ARE_DEFINED
  ToolboxNode(I18n::Message::Matrices,  I18n::Message::Default, I18n::Message::Default, matricesChildren, 5),
#endif
#if LIST_ARE_DEFINED
  ToolboxNode(I18n::Message::Lists, I18n::Message::Default, I18n::Message::Default, listesChildren, 5),
#endif
  ToolboxNode(I18n::Message::Approximation, I18n::Message::Default, I18n::Message::Default, approximationChildren, 4),
  ToolboxNode(I18n::Message::HyperbolicTrigonometry, I18n::Message::Default, I18n::Message::Default, trigonometryChildren, 6),
  ToolboxNode(I18n::Message::Fluctuation, I18n::Message::Default, I18n::Message::Default, predictionChildren, 3)};
#if LIST_ARE_DEFINED
const ToolboxNode toolboxModel = ToolboxNode(I18n::Message::Toolbox, I18n::Message::Default, I18n::Message::Default, menu, 12);
#elif MATRICES_ARE_DEFINED
const ToolboxNode toolboxModel = ToolboxNode(I18n::Message::Toolbox, I18n::Message::Default, I18n::Message::Default, menu, 11);
#else
const ToolboxNode toolboxModel = ToolboxNode(I18n::Message::Toolbox, I18n::Message::Default, I18n::Message::Default, menu, 10);
#endif

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
  m_selectableTableView(&m_listController, this, 0, 1, 0, 0, 0, 0, this, nullptr, false),
  m_listController(this, &m_selectableTableView),
  m_nodeModel(nullptr)
{
}

void MathToolbox::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_listController);
}

bool MathToolbox::handleEvent(Ion::Events::Event event) {
  return handleEventForRow(event, selectedRow());
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

void MathToolbox::viewDidDisappear() {
  Toolbox::viewDidDisappear();
  m_selectableTableView.deselectTable();
}

int MathToolbox::stackDepth() {
  return m_stack.depth();
}

TextField * MathToolbox::sender() {
  return (TextField *)Toolbox::sender();
}

bool MathToolbox::handleEventForRow(Ion::Events::Event event, int selectedRow) {
  int depth = m_stack.depth();
  if (event == Ion::Events::Back && depth == 0) {
    m_selectableTableView.deselectTable();
    app()->dismissModalViewController();
    return true;
  }
  if ((event == Ion::Events::Back || event == Ion::Events::Left) && depth > 0) {
    return returnToPreviousMenu();
  }
  ToolboxNode * selectedNode = (ToolboxNode *)m_nodeModel->children(selectedRow);
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) && selectedNode->numberOfChildren() > 0) {
    return selectSubMenu(selectedNode);
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedNode->numberOfChildren() == 0) {
    return selectLeaf(selectedNode);
  }
  return false;
}

const ToolboxNode * MathToolbox::rootModel() {
  return &toolboxModel;
}

bool MathToolbox::selectLeaf(ToolboxNode * selectedNode){
  m_selectableTableView.deselectTable();
  ToolboxNode * node = selectedNode;
  const char * editedText = I18n::translate(node->insertedText());
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
  m_stack.push(selectedRow(),  m_selectableTableView.contentOffset().y());
  m_selectableTableView.deselectTable();
  m_nodeModel = selectedNode;
  m_listController.setFirstSelectedRow(0);
  app()->setFirstResponder(&m_listController);
  return true;
}
