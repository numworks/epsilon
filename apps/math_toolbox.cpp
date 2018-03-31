#include "math_toolbox.h"
#include "./shared/toolbox_helpers.h"
#include <assert.h>
#include <string.h>

/* TODO: find a shorter way to initialize tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at I18n::Message::Default. */

const ToolboxMessageTree calculChildren[4] = {
  ToolboxMessageTree(I18n::Message::DiffCommandWithArg, I18n::Message::DerivateNumber, I18n::Message::DiffCommandWithArg),
  ToolboxMessageTree(I18n::Message::IntCommandWithArg, I18n::Message::Integral, I18n::Message::IntCommandWithArg),
  ToolboxMessageTree(I18n::Message::SumCommandWithArg, I18n::Message::Sum, I18n::Message::SumCommandWithArg),
  ToolboxMessageTree(I18n::Message::ProductCommandWithArg, I18n::Message::Product, I18n::Message::ProductCommandWithArg)};

const ToolboxMessageTree complexChildren[5] = {
  ToolboxMessageTree(I18n::Message::AbsCommandWithArg,I18n::Message::ComplexAbsoluteValue, I18n::Message::AbsCommandWithArg),
  ToolboxMessageTree(I18n::Message::ArgCommandWithArg, I18n::Message::Agument, I18n::Message::ArgCommandWithArg),
  ToolboxMessageTree(I18n::Message::ReCommandWithArg, I18n::Message::RealPart, I18n::Message::ReCommandWithArg),
  ToolboxMessageTree(I18n::Message::ImCommandWithArg, I18n::Message::ImaginaryPart, I18n::Message::ImCommandWithArg),
  ToolboxMessageTree(I18n::Message::ConjCommandWithArg, I18n::Message::Conjugate, I18n::Message::ConjCommandWithArg)};

const ToolboxMessageTree probabilityChildren[2] = {
  ToolboxMessageTree(I18n::Message::BinomialCommandWithArg, I18n::Message::Combination, I18n::Message::BinomialCommandWithArg),
  ToolboxMessageTree(I18n::Message::PermuteCommandWithArg, I18n::Message::Permutation, I18n::Message::PermuteCommandWithArg)};

const ToolboxMessageTree arithmeticChildren[5] = {
  ToolboxMessageTree(I18n::Message::GcdCommandWithArg, I18n::Message::GreatCommonDivisor, I18n::Message::GcdCommandWithArg),
  ToolboxMessageTree(I18n::Message::LcmCommandWithArg, I18n::Message::LeastCommonMultiple, I18n::Message::LcmCommandWithArg),
  ToolboxMessageTree(I18n::Message::FactorCommandWithArg, I18n::Message::PrimeFactorDecomposition, I18n::Message::FactorCommandWithArg),
  ToolboxMessageTree(I18n::Message::RemCommandWithArg, I18n::Message::Remainder, I18n::Message::RemCommandWithArg),
  ToolboxMessageTree(I18n::Message::QuoCommandWithArg, I18n::Message::Quotient, I18n::Message::QuoCommandWithArg)};

#if MATRICES_ARE_DEFINED
const ToolboxMessageTree matricesChildren[5] = {
  ToolboxMessageTree(I18n::Message::InverseCommandWithArg, I18n::Message::Inverse, I18n::Message::InverseCommandWithArg),
  ToolboxMessageTree(I18n::Message::DeterminantCommandWithArg, I18n::Message::Determinant, I18n::Message::DeterminantCommandWithArg),
  ToolboxMessageTree(I18n::Message::TransposeCommandWithArg, I18n::Message::Transpose, I18n::Message::TransposeCommandWithArg),
  ToolboxMessageTree(I18n::Message::TraceCommandWithArg, I18n::Message::Trace, I18n::Message::TraceCommandWithArg),
  ToolboxMessageTree(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension, I18n::Message::DimensionCommandWithArg)};
#endif

#if LIST_ARE_DEFINED
const ToolboxMessageTree listesChildren[5] = {
  ToolboxMessageTree(I18n::Message::SortCommandWithArg, I18n::Message::Sort, I18n::Message::SortCommandWithArg),
  ToolboxMessageTree(I18n::Message::InvSortCommandWithArg, I18n::Message::InvSort, I18n::Message::InvSortCommandWithArg),
  ToolboxMessageTree(I18n::Message::MaxCommandWithArg, I18n::Message::Maximum, I18n::Message::MaxCommandWithArg),
  ToolboxMessageTree(I18n::Message::MinCommandWithArg, I18n::Message::Minimum, I18n::Message::MinCommandWithArg),
  ToolboxMessageTree(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension, I18n::Message::DimensionCommandWithArg)};
#endif

const ToolboxMessageTree randomAndApproximationChildren[6] = {
  ToolboxMessageTree(I18n::Message::RandomCommandWithArg, I18n::Message::RandomFloat, I18n::Message::RandomCommandWithArg),
  ToolboxMessageTree(I18n::Message::RandintCommandWithArg, I18n::Message::RandomInteger, I18n::Message::RandintCommandWithArg),
  ToolboxMessageTree(I18n::Message::FloorCommandWithArg, I18n::Message::Floor, I18n::Message::FloorCommandWithArg),
  ToolboxMessageTree(I18n::Message::FracCommandWithArg, I18n::Message::FracPart, I18n::Message::FracCommandWithArg),
  ToolboxMessageTree(I18n::Message::CeilCommandWithArg, I18n::Message::Ceiling, I18n::Message::CeilCommandWithArg),
  ToolboxMessageTree(I18n::Message::RoundCommandWithArg, I18n::Message::Rounding, I18n::Message::RoundCommandWithArg)};

const ToolboxMessageTree trigonometryChildren[6] = {
  ToolboxMessageTree(I18n::Message::CoshCommandWithArg, I18n::Message::HyperbolicCosine, I18n::Message::CoshCommandWithArg),
  ToolboxMessageTree(I18n::Message::SinhCommandWithArg, I18n::Message::HyperbolicSine, I18n::Message::SinhCommandWithArg),
  ToolboxMessageTree(I18n::Message::TanhCommandWithArg, I18n::Message::HyperbolicTangent, I18n::Message::TanhCommandWithArg),
  ToolboxMessageTree(I18n::Message::AcoshCommandWithArg, I18n::Message::InverseHyperbolicCosine, I18n::Message::AcoshCommandWithArg),
  ToolboxMessageTree(I18n::Message::AsinhCommandWithArg, I18n::Message::InverseHyperbolicSine, I18n::Message::AsinhCommandWithArg),
  ToolboxMessageTree(I18n::Message::AtanhCommandWithArg, I18n::Message::InverseHyperbolicTangent, I18n::Message::AtanhCommandWithArg)};

const ToolboxMessageTree predictionChildren[3] = {
  ToolboxMessageTree(I18n::Message::Prediction95CommandWithArg, I18n::Message::Prediction95, I18n::Message::Prediction95CommandWithArg),
  ToolboxMessageTree(I18n::Message::PredictionCommandWithArg, I18n::Message::Prediction, I18n::Message::PredictionCommandWithArg),
  ToolboxMessageTree(I18n::Message::ConfidenceCommandWithArg, I18n::Message::Confidence, I18n::Message::ConfidenceCommandWithArg)};

#if LIST_ARE_DEFINED
const ToolboxMessageTree menu[12] = {ToolboxMessageTree(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue, I18n::Message::AbsCommandWithArg),
#elif MATRICES_ARE_DEFINED
const ToolboxMessageTree menu[11] = {ToolboxMessageTree(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue, I18n::Message::AbsCommandWithArg),
#else
const ToolboxMessageTree menu[10] = {ToolboxMessageTree(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue, I18n::Message::AbsCommandWithArg),
#endif
  ToolboxMessageTree(I18n::Message::RootCommandWithArg, I18n::Message::NthRoot, I18n::Message::RootCommandWithArg),
  ToolboxMessageTree(I18n::Message::LogCommandWithArg, I18n::Message::BasedLogarithm, I18n::Message::LogCommandWithArg),
  ToolboxMessageTree(I18n::Message::Calculation, I18n::Message::Default, I18n::Message::Default, calculChildren, 4),
  ToolboxMessageTree(I18n::Message::ComplexNumber, I18n::Message::Default, I18n::Message::Default, complexChildren, 5),
  ToolboxMessageTree(I18n::Message::Probability, I18n::Message::Default, I18n::Message::Default, probabilityChildren, 2),
  ToolboxMessageTree(I18n::Message::Arithmetic, I18n::Message::Default, I18n::Message::Default, arithmeticChildren, 5),
#if MATRICES_ARE_DEFINED
  ToolboxMessageTree(I18n::Message::Matrices,  I18n::Message::Default, I18n::Message::Default, matricesChildren, 5),
#endif
#if LIST_ARE_DEFINED
  ToolboxMessageTree(I18n::Message::Lists, I18n::Message::Default, I18n::Message::Default, listesChildren, 5),
#endif
  ToolboxMessageTree(I18n::Message::RandomAndApproximation, I18n::Message::Default, I18n::Message::Default, randomAndApproximationChildren, 6),
  ToolboxMessageTree(I18n::Message::HyperbolicTrigonometry, I18n::Message::Default, I18n::Message::Default, trigonometryChildren, 6),
  ToolboxMessageTree(I18n::Message::Fluctuation, I18n::Message::Default, I18n::Message::Default, predictionChildren, 3)};
#if LIST_ARE_DEFINED
const ToolboxMessageTree toolboxModel = ToolboxMessageTree(I18n::Message::Toolbox, I18n::Message::Default, I18n::Message::Default, menu, 12);
#elif MATRICES_ARE_DEFINED
const ToolboxMessageTree toolboxModel = ToolboxMessageTree(I18n::Message::Toolbox, I18n::Message::Default, I18n::Message::Default, menu, 11);
#else
const ToolboxMessageTree toolboxModel = ToolboxMessageTree(I18n::Message::Toolbox, I18n::Message::Default, I18n::Message::Default, menu, 10);
#endif

MathToolbox::MathToolbox() : Toolbox(nullptr, I18n::translate(rootModel()->label()))
{
}

bool MathToolbox::handleEvent(Ion::Events::Event event) {
  if (Toolbox::handleEvent(event)) {
    return true;
  }
  if (event.hasText() && strlen(event.text()) == 1) {
    char key = event.text()[0];
    if (key >= '0' && key <= '9') {
      int index = key == '0' ? 9 : key - '0' - 1;
      if (index >= 0 && index < m_messageTreeModel->numberOfChildren()) {
        scrollToAndSelectChild(index);
        return true;
      }
    }
  }
  return false;
}

TextField * MathToolbox::sender() {
  return (TextField *)Toolbox::sender();
}

bool MathToolbox::selectLeaf(ToolboxMessageTree * selectedMessageTree) {
  m_selectableTableView.deselectTable();
  ToolboxMessageTree * messageTree = selectedMessageTree;
  const char * editedText = I18n::translate(messageTree->insertedText());
  char strippedEditedText[strlen(editedText)];
  Shared::ToolboxHelpers::TextToInsertForCommandMessage(messageTree->insertedText(), strippedEditedText);
  sender()->handleEventWithText(strippedEditedText);
  app()->dismissModalViewController();
  return true;
}

const ToolboxMessageTree * MathToolbox::rootModel() {
  return &toolboxModel;
}

MessageTableCellWithMessage * MathToolbox::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

MessageTableCellWithChevron* MathToolbox::nodeCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_nodeCells[index];
}

int MathToolbox::maxNumberOfDisplayedRows() {
 return k_maxNumberOfDisplayedRows;
}

void MathToolbox::scrollToAndSelectChild(int i) {
  assert(i >=0 && i<m_messageTreeModel->numberOfChildren());
  m_selectableTableView.deselectTable();
  m_selectableTableView.scrollToCell(0, i);
  m_selectableTableView.selectCellAtLocation(0, i);
}
