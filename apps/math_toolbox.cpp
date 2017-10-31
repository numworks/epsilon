#include "math_toolbox.h"
#include "./shared/toolbox_helpers.h"
#include <assert.h>
#include <string.h>

/* TODO: find a shorter way to initialize tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at I18n::Message::Default. */

const ToolboxMessageTree calculChildren[4] = {ToolboxMessageTree(I18n::Message::DiffCommandWithArg, I18n::Message::DerivateNumber, I18n::Message::DiffCommand), ToolboxMessageTree(I18n::Message::IntCommandWithArg, I18n::Message::Integral, I18n::Message::IntCommand), ToolboxMessageTree(I18n::Message::SumCommandWithArg, I18n::Message::Sum, I18n::Message::SumCommand), ToolboxMessageTree(I18n::Message::ProductCommandWithArg, I18n::Message::Product, I18n::Message::ProductCommand)};
const ToolboxMessageTree complexChildren[5] = {ToolboxMessageTree(I18n::Message::AbsCommandWithArg, I18n::Message::ComplexAbsoluteValue, I18n::Message::AbsCommand), ToolboxMessageTree(I18n::Message::ArgCommandWithArg, I18n::Message::Agument, I18n::Message::ArgCommand), ToolboxMessageTree(I18n::Message::ReCommandWithArg, I18n::Message::RealPart, I18n::Message::ReCommand), ToolboxMessageTree(I18n::Message::ImCommandWithArg, I18n::Message::ImaginaryPart, I18n::Message::ImCommand), ToolboxMessageTree(I18n::Message::ConjCommandWithArg, I18n::Message::Conjugate, I18n::Message::ConjCommand)};
const ToolboxMessageTree probabilityChildren[2] = {ToolboxMessageTree(I18n::Message::BinomialCommandWithArg, I18n::Message::Combination, I18n::Message::BinomialCommand), ToolboxMessageTree(I18n::Message::PermuteCommandWithArg, I18n::Message::Permutation, I18n::Message::PermuteCommand)};
const ToolboxMessageTree arithmeticChildren[4] = {ToolboxMessageTree(I18n::Message::GcdCommandWithArg, I18n::Message::GreatCommonDivisor, I18n::Message::GcdCommand),ToolboxMessageTree(I18n::Message::LcmCommandWithArg, I18n::Message::LeastCommonMultiple, I18n::Message::LcmCommand), ToolboxMessageTree(I18n::Message::RemCommandWithArg, I18n::Message::Remainder, I18n::Message::RemCommand), ToolboxMessageTree(I18n::Message::QuoCommandWithArg, I18n::Message::Quotient, I18n::Message::QuoCommand)};
#if MATRICES_ARE_DEFINED
const ToolboxMessageTree matricesChildren[5] = {ToolboxMessageTree(I18n::Message::InverseCommandWithArg, I18n::Message::Inverse, I18n::Message::InverseCommand), ToolboxMessageTree(I18n::Message::DeterminantCommandWithArg, I18n::Message::Determinant, I18n::Message::DeterminantCommand), ToolboxMessageTree(I18n::Message::TransposeCommandWithArg, I18n::Message::Transpose, I18n::Message::TransposeCommand), ToolboxMessageTree(I18n::Message::TraceCommandWithArg, I18n::Message::Trace, I18n::Message::TraceCommand), ToolboxMessageTree(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension, I18n::Message::DimensionCommand)};
#endif
#if LIST_ARE_DEFINED
const ToolboxMessageTree listesChildren[5] = {ToolboxMessageTree(I18n::Message::SortCommandWithArg, I18n::Message::Sort, I18n::Message::SortCommand), ToolboxMessageTree(I18n::Message::InvSortCommandWithArg, I18n::Message::InvSort, I18n::Message::InvSortCommand), ToolboxMessageTree(I18n::Message::MaxCommandWithArg, I18n::Message::Maximum, I18n::Message::MaxCommand), ToolboxMessageTree(I18n::Message::MinCommandWithArg, I18n::Message::Minimum, I18n::Message::MinCommand), ToolboxMessageTree(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension, I18n::Message::DimensionCommand)};
#endif
const ToolboxMessageTree approximationChildren[4] = {ToolboxMessageTree(I18n::Message::FloorCommandWithArg, I18n::Message::Floor, I18n::Message::FloorCommand), ToolboxMessageTree(I18n::Message::FracCommandWithArg, I18n::Message::FracPart, I18n::Message::FracCommand), ToolboxMessageTree(I18n::Message::CeilCommandWithArg, I18n::Message::Ceiling, I18n::Message::CeilCommand), ToolboxMessageTree(I18n::Message::RoundCommandWithArg, I18n::Message::Rounding, I18n::Message::RoundCommand)};
const ToolboxMessageTree trigonometryChildren[6] = {ToolboxMessageTree(I18n::Message::CoshCommandWithArg, I18n::Message::HyperbolicCosine, I18n::Message::CoshCommand), ToolboxMessageTree(I18n::Message::SinhCommandWithArg, I18n::Message::HyperbolicSine, I18n::Message::SinhCommand), ToolboxMessageTree(I18n::Message::TanhCommandWithArg, I18n::Message::HyperbolicTangent, I18n::Message::TanhCommand), ToolboxMessageTree(I18n::Message::AcoshCommandWithArg, I18n::Message::InverseHyperbolicCosine, I18n::Message::AcoshCommand), ToolboxMessageTree(I18n::Message::AsinhCommandWithArg, I18n::Message::InverseHyperbolicSine, I18n::Message::AsinhCommand), ToolboxMessageTree(I18n::Message::AtanhCommandWithArg, I18n::Message::InverseHyperbolicTangent, I18n::Message::AtanhCommand)};
const ToolboxMessageTree predictionChildren[3] = {ToolboxMessageTree(I18n::Message::Prediction95CommandWithArg, I18n::Message::Prediction95, I18n::Message::Prediction95Command), ToolboxMessageTree(I18n::Message::PredictionCommandWithArg, I18n::Message::Prediction, I18n::Message::PredictionCommand), ToolboxMessageTree(I18n::Message::ConfidenceCommandWithArg, I18n::Message::Confidence, I18n::Message::ConfidenceCommand)};

#if LIST_ARE_DEFINED
const ToolboxMessageTree menu[12] = {ToolboxMessageTree(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue, I18n::Message::AbsCommand),
#elif MATRICES_ARE_DEFINED
const ToolboxMessageTree menu[11] = {ToolboxMessageTree(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue, I18n::Message::AbsCommand),
#else
const ToolboxMessageTree menu[10] = {ToolboxMessageTree(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue, I18n::Message::AbsCommand),
#endif
  ToolboxMessageTree(I18n::Message::RootCommandWithArg, I18n::Message::NthRoot, I18n::Message::RootCommand),
  ToolboxMessageTree(I18n::Message::LogCommandWithArg, I18n::Message::BasedLogarithm, I18n::Message::LogCommand),
  ToolboxMessageTree(I18n::Message::Calculation, I18n::Message::Default, I18n::Message::Default, calculChildren, 4),
  ToolboxMessageTree(I18n::Message::ComplexNumber, I18n::Message::Default, I18n::Message::Default, complexChildren, 5),
  ToolboxMessageTree(I18n::Message::Probability, I18n::Message::Default, I18n::Message::Default, probabilityChildren, 2),
  ToolboxMessageTree(I18n::Message::Arithmetic, I18n::Message::Default, I18n::Message::Default, arithmeticChildren, 4),
#if MATRICES_ARE_DEFINED
  ToolboxMessageTree(I18n::Message::Matrices,  I18n::Message::Default, I18n::Message::Default, matricesChildren, 5),
#endif
#if LIST_ARE_DEFINED
  ToolboxMessageTree(I18n::Message::Lists, I18n::Message::Default, I18n::Message::Default, listesChildren, 5),
#endif
  ToolboxMessageTree(I18n::Message::Approximation, I18n::Message::Default, I18n::Message::Default, approximationChildren, 4),
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

KDCoordinate MathToolbox::rowHeight(int j) {
  if (typeAtLocation(0, j) == Toolbox::LeafCellType) {
    return k_leafRowHeight;
  }
  return k_nodeRowHeight;
}

TextField * MathToolbox::sender() {
  return (TextField *)Toolbox::sender();
}

bool MathToolbox::selectLeaf(ToolboxMessageTree * selectedMessageTree) {
  m_selectableTableView.deselectTable();
  ToolboxMessageTree * messageTree = selectedMessageTree;
  const char * editedText = I18n::translate(messageTree->insertedText());
  if (!sender()->isEditing()) {
    sender()->setEditing(true);
  }
  sender()->insertTextAtLocation(editedText, sender()->cursorLocation());
  int newCursorLocation = sender()->cursorLocation() + Shared::ToolboxHelpers::CursorIndexInCommand(editedText);
  sender()->setCursorLocation(newCursorLocation);
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
