#include "math_toolbox.h"
#include <assert.h>
#include <string.h>

/* TODO: find a shorter way to initialize tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at &I18n::Common::Default. */

const ToolboxNode calculChildren[4] = {ToolboxNode(&I18n::Common::DiffCommandWithArg, &I18n::Common::DerivateNumber, &I18n::Common::DiffCommand), ToolboxNode(&I18n::Common::IntCommandWithArg, &I18n::Common::Integral, &I18n::Common::IntCommand), ToolboxNode(&I18n::Common::SumCommandWithArg, &I18n::Common::Sum, &I18n::Common::SumCommand), ToolboxNode(&I18n::Common::ProductCommandWithArg, &I18n::Common::Product, &I18n::Common::ProductCommand)};
const ToolboxNode complexChildren[5] = {ToolboxNode(&I18n::Common::AbsCommandWithArg, &I18n::Common::ComplexAbsoluteValue, &I18n::Common::AbsCommand), ToolboxNode(&I18n::Common::ArgCommandWithArg, &I18n::Common::Agument, &I18n::Common::ArgCommand), ToolboxNode(&I18n::Common::ReCommandWithArg, &I18n::Common::ReelPart, &I18n::Common::ReCommand), ToolboxNode(&I18n::Common::ImCommandWithArg, &I18n::Common::ImaginaryPart, &I18n::Common::ImCommand), ToolboxNode(&I18n::Common::ConjCommandWithArg, &I18n::Common::Conjugate, &I18n::Common::ConjCommand)};
const ToolboxNode probabilityChildren[2] = {ToolboxNode(&I18n::Common::BinomialCommandWithArg, &I18n::Common::Combination, &I18n::Common::BinomialCommand), ToolboxNode(&I18n::Common::PermuteCommandWithArg, &I18n::Common::Permutation, &I18n::Common::PermuteCommand)};
const ToolboxNode arithmeticChildren[4] = {ToolboxNode(&I18n::Common::GcdCommandWithArg, &I18n::Common::GreatCommonDivisor, &I18n::Common::GcdCommand),ToolboxNode(&I18n::Common::LcmCommandWithArg, &I18n::Common::LeastCommonMultiple, &I18n::Common::LcmCommand), ToolboxNode(&I18n::Common::RemCommandWithArg, &I18n::Common::Remainder, &I18n::Common::RemCommand), ToolboxNode(&I18n::Common::QuoCommandWithArg, &I18n::Common::Quotient, &I18n::Common::QuoCommand)};
#if MATRICES_ARE_DEFINED
const ToolboxNode matricesChildren[5] = {ToolboxNode(&I18n::Common::InverseCommandWithArg, &I18n::Common::Inverse, &I18n::Common::InverseCommand), ToolboxNode(&I18n::Common::DeterminantCommandWithArg, &I18n::Common::Determinant, &I18n::Common::DeterminantCommand), ToolboxNode(&I18n::Common::TransposeCommandWithArg, &I18n::Common::Transpose, &I18n::Common::TransposeCommand), ToolboxNode(&I18n::Common::TraceCommandWithArg, &I18n::Common::Trace, &I18n::Common::TraceCommand), ToolboxNode(&I18n::Common::DimensionCommandWithArg, &I18n::Common::Dimension, &I18n::Common::DimensionCommand)};
#endif
#if LIST_ARE_DEFINED
const ToolboxNode listesChildren[5] = {ToolboxNode(&I18n::Common::SortCommandWithArg, &I18n::Common::Sort, &I18n::Common::SortCommand), ToolboxNode(&I18n::Common::InvSortCommandWithArg, &I18n::Common::InvSort, &I18n::Common::InvSortCommand), ToolboxNode(&I18n::Common::MaxCommandWithArg, &I18n::Common::Maximum, &I18n::Common::MaxCommand), ToolboxNode(&I18n::Common::MinCommandWithArg, &I18n::Common::Minimum, &I18n::Common::MinCommand), ToolboxNode(&I18n::Common::DimensionCommandWithArg, &I18n::Common::Dimension, &I18n::Common::DimensionCommand)};
#endif
const ToolboxNode approximationChildren[4] = {ToolboxNode(&I18n::Common::FloorCommandWithArg, &I18n::Common::Floor, &I18n::Common::FloorCommand), ToolboxNode(&I18n::Common::FracCommandWithArg, &I18n::Common::FracPart, &I18n::Common::FracCommand), ToolboxNode(&I18n::Common::CeilCommandWithArg, &I18n::Common::Ceiling, &I18n::Common::CeilCommand), ToolboxNode(&I18n::Common::RoundCommandWithArg, &I18n::Common::Rounding, &I18n::Common::RoundCommand)};
const ToolboxNode trigonometryChildren[6] = {ToolboxNode(&I18n::Common::CoshCommandWithArg, &I18n::Common::HyperbolicCosine, &I18n::Common::CoshCommand), ToolboxNode(&I18n::Common::SinhCommandWithArg, &I18n::Common::HyperbolicSine, &I18n::Common::SinhCommand), ToolboxNode(&I18n::Common::TanhCommandWithArg, &I18n::Common::HyperbolicTangent, &I18n::Common::TanhCommand), ToolboxNode(&I18n::Common::AcoshCommandWithArg, &I18n::Common::InverseHyperbolicCosine, &I18n::Common::AcoshCommand), ToolboxNode(&I18n::Common::AsinhCommandWithArg, &I18n::Common::InverseHyperbolicSine, &I18n::Common::AsinhCommand), ToolboxNode(&I18n::Common::AtanhCommandWithArg, &I18n::Common::InverseHyperbolicTangent, &I18n::Common::AtanhCommand)};
const ToolboxNode predictionChildren[3] = {ToolboxNode(&I18n::Common::Prediction95CommandWithArg, &I18n::Common::Prediction95, &I18n::Common::Prediction95Command), ToolboxNode(&I18n::Common::PredictionCommandWithArg, &I18n::Common::Prediction, &I18n::Common::PredictionCommand), ToolboxNode(&I18n::Common::ConfidenceCommandWithArg, &I18n::Common::Confidence, &I18n::Common::ConfidenceCommand)};

#if LIST_ARE_DEFINED
const ToolboxNode menu[12] = {ToolboxNode(&I18n::Common::AbsCommandWithArg, &I18n::Common::AbsoluteValue, &I18n::Common::AbsCommand),
#elif MATRICES_ARE_DEFINED
const ToolboxNode menu[11] = {ToolboxNode(&I18n::Common::AbsCommandWithArg, &I18n::Common::AbsoluteValue, &I18n::Common::AbsCommand),
#else
const ToolboxNode menu[10] = {ToolboxNode(&I18n::Common::AbsCommandWithArg, &I18n::Common::AbsoluteValue, &I18n::Common::AbsCommand),
#endif
  ToolboxNode(&I18n::Common::RootCommandWithArg, &I18n::Common::NthRoot, &I18n::Common::RootCommand),
  ToolboxNode(&I18n::Common::LogCommandWithArg, &I18n::Common::BasedLogarithm, &I18n::Common::LogCommand),
  ToolboxNode(&I18n::Common::Calculation, &I18n::Common::Default, &I18n::Common::Default, calculChildren, 4),
  ToolboxNode(&I18n::Common::ComplexNumber, &I18n::Common::Default, &I18n::Common::Default, complexChildren, 5),
  ToolboxNode(&I18n::Common::Probability, &I18n::Common::Default, &I18n::Common::Default, probabilityChildren, 2),
  ToolboxNode(&I18n::Common::Arithmetic, &I18n::Common::Default, &I18n::Common::Default, arithmeticChildren, 4),
#if MATRICES_ARE_DEFINED
  ToolboxNode(&I18n::Common::Matrices,  &I18n::Common::Default, &I18n::Common::Default, matricesChildren, 5),
#endif
#if LIST_ARE_DEFINED
  ToolboxNode(&I18n::Common::Lists, &I18n::Common::Default, &I18n::Common::Default, listesChildren, 5),
#endif
  ToolboxNode(&I18n::Common::Approximation, &I18n::Common::Default, &I18n::Common::Default, approximationChildren, 4),
  ToolboxNode(&I18n::Common::HyperbolicTrigonometry, &I18n::Common::Default, &I18n::Common::Default, trigonometryChildren, 6),
  ToolboxNode(&I18n::Common::Fluctuation, &I18n::Common::Default, &I18n::Common::Default, predictionChildren, 3)};
#if LIST_ARE_DEFINED
const ToolboxNode toolboxModel = ToolboxNode(&I18n::Common::Toolbox, &I18n::Common::Default, &I18n::Common::Default, menu, 12);
#elif MATRICES_ARE_DEFINED
const ToolboxNode toolboxModel = ToolboxNode(&I18n::Common::Toolbox, &I18n::Common::Default, &I18n::Common::Default, menu, 11);
#else
const ToolboxNode toolboxModel = ToolboxNode(&I18n::Common::Toolbox, &I18n::Common::Default, &I18n::Common::Default, menu, 10);
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
