#include "math_toolbox.h"
#include "./shared/toolbox_helpers.h"
#include <assert.h>
#include <string.h>

using namespace Poincare;

/* We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at I18n::Message::Default. */

const ToolboxMessageTree calculChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::DiffCommandWithArg, I18n::Message::DerivateNumber, false, I18n::Message::DiffCommand),
  ToolboxMessageTree::Leaf(I18n::Message::IntCommandWithArg, I18n::Message::Integral, false, I18n::Message::IntCommand),
  ToolboxMessageTree::Leaf(I18n::Message::SumCommandWithArg, I18n::Message::Sum, false, I18n::Message::SumCommand),
  ToolboxMessageTree::Leaf(I18n::Message::ProductCommandWithArg, I18n::Message::Product, false, I18n::Message::ProductCommand)
};

const ToolboxMessageTree complexChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AbsCommandWithArg,I18n::Message::ComplexAbsoluteValue),
  ToolboxMessageTree::Leaf(I18n::Message::ArgCommandWithArg, I18n::Message::Agument),
  ToolboxMessageTree::Leaf(I18n::Message::ReCommandWithArg, I18n::Message::RealPart),
  ToolboxMessageTree::Leaf(I18n::Message::ImCommandWithArg, I18n::Message::ImaginaryPart),
  ToolboxMessageTree::Leaf(I18n::Message::ConjCommandWithArg, I18n::Message::Conjugate)
};

const ToolboxMessageTree combinatoricsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::BinomialCommandWithArg, I18n::Message::Combination),
  ToolboxMessageTree::Leaf(I18n::Message::PermuteCommandWithArg, I18n::Message::Permutation)
};

const ToolboxMessageTree normalDistributionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::NormCDFCommandWithArg, I18n::Message::NormCDF),
  ToolboxMessageTree::Leaf(I18n::Message::NormCDF2CommandWithArg, I18n::Message::NormCDF2),
  ToolboxMessageTree::Leaf(I18n::Message::InvNormCommandWithArg, I18n::Message::InvNorm),
  ToolboxMessageTree::Leaf(I18n::Message::NormPDFCommandWithArg, I18n::Message::NormPDF)
};

const ToolboxMessageTree binomialDistributionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::BinomialPDFCommandWithArg, I18n::Message::BinomialPDF),
  ToolboxMessageTree::Leaf(I18n::Message::BinomialCDFCommandWithArg, I18n::Message::BinomialCDF),
  ToolboxMessageTree::Leaf(I18n::Message::InvBinomialCommandWithArg, I18n::Message::InvBinomial),
};

const ToolboxMessageTree probabilityChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::NormalDistribution, normalDistributionChildren),
  ToolboxMessageTree::Node(I18n::Message::BinomialDistribution, binomialDistributionChildren)};

const ToolboxMessageTree arithmeticChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::GcdCommandWithArg, I18n::Message::GreatCommonDivisor),
  ToolboxMessageTree::Leaf(I18n::Message::LcmCommandWithArg, I18n::Message::LeastCommonMultiple),
  ToolboxMessageTree::Leaf(I18n::Message::FactorCommandWithArg, I18n::Message::PrimeFactorDecomposition),
  ToolboxMessageTree::Leaf(I18n::Message::RemCommandWithArg, I18n::Message::Remainder),
  ToolboxMessageTree::Leaf(I18n::Message::QuoCommandWithArg, I18n::Message::Quotient)
};

const ToolboxMessageTree matricesChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::MatrixCommandWithArg, I18n::Message::NewMatrix, false, I18n::Message::MatrixCommand),
  ToolboxMessageTree::Leaf(I18n::Message::IndentityCommandWithArg, I18n::Message::Identity),
  ToolboxMessageTree::Leaf(I18n::Message::InverseCommandWithArg, I18n::Message::Inverse),
  ToolboxMessageTree::Leaf(I18n::Message::DeterminantCommandWithArg, I18n::Message::Determinant),
  ToolboxMessageTree::Leaf(I18n::Message::TransposeCommandWithArg, I18n::Message::Transpose),
  ToolboxMessageTree::Leaf(I18n::Message::TraceCommandWithArg, I18n::Message::Trace),
  ToolboxMessageTree::Leaf(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension)
};

#if LIST_ARE_DEFINED
const ToolboxMessageTree listsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::SortCommandWithArg, I18n::Message::Sort),
  ToolboxMessageTree::Leaf(I18n::Message::InvSortCommandWithArg, I18n::Message::InvSort),
  ToolboxMessageTree::Leaf(I18n::Message::MaxCommandWithArg, I18n::Message::Maximum),
  ToolboxMessageTree::Leaf(I18n::Message::MinCommandWithArg, I18n::Message::Minimum),
  ToolboxMessageTree::Leaf(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension)
};
#endif

const ToolboxMessageTree randomAndApproximationChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::RandomCommandWithArg, I18n::Message::RandomFloat),
  ToolboxMessageTree::Leaf(I18n::Message::RandintCommandWithArg, I18n::Message::RandomInteger),
  ToolboxMessageTree::Leaf(I18n::Message::FloorCommandWithArg, I18n::Message::Floor),
  ToolboxMessageTree::Leaf(I18n::Message::FracCommandWithArg, I18n::Message::FracPart),
  ToolboxMessageTree::Leaf(I18n::Message::CeilCommandWithArg, I18n::Message::Ceiling),
  ToolboxMessageTree::Leaf(I18n::Message::RoundCommandWithArg, I18n::Message::Rounding)};

const ToolboxMessageTree trigonometryChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::CoshCommandWithArg, I18n::Message::HyperbolicCosine),
  ToolboxMessageTree::Leaf(I18n::Message::SinhCommandWithArg, I18n::Message::HyperbolicSine),
  ToolboxMessageTree::Leaf(I18n::Message::TanhCommandWithArg, I18n::Message::HyperbolicTangent),
  ToolboxMessageTree::Leaf(I18n::Message::AcoshCommandWithArg, I18n::Message::InverseHyperbolicCosine),
  ToolboxMessageTree::Leaf(I18n::Message::AsinhCommandWithArg, I18n::Message::InverseHyperbolicSine),
  ToolboxMessageTree::Leaf(I18n::Message::AtanhCommandWithArg, I18n::Message::InverseHyperbolicTangent)};

const ToolboxMessageTree predictionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::Prediction95CommandWithArg, I18n::Message::Prediction95),
  ToolboxMessageTree::Leaf(I18n::Message::PredictionCommandWithArg, I18n::Message::Prediction),
  ToolboxMessageTree::Leaf(I18n::Message::ConfidenceCommandWithArg, I18n::Message::Confidence)};

const ToolboxMessageTree menu[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue),
  ToolboxMessageTree::Leaf(I18n::Message::RootCommandWithArg, I18n::Message::NthRoot),
  ToolboxMessageTree::Leaf(I18n::Message::LogCommandWithArg, I18n::Message::BasedLogarithm),
  ToolboxMessageTree::Node(I18n::Message::Calculation, calculChildren),
  ToolboxMessageTree::Node(I18n::Message::ComplexNumber, complexChildren),
  ToolboxMessageTree::Node(I18n::Message::Combinatorics, combinatoricsChildren),
  ToolboxMessageTree::Node(I18n::Message::Probability, probabilityChildren),
  ToolboxMessageTree::Node(I18n::Message::Arithmetic, arithmeticChildren),
  ToolboxMessageTree::Node(I18n::Message::Matrices, matricesChildren),
#if LIST_ARE_DEFINED
  ToolboxMessageTree::Node(I18n::Message::Lists,listsChildren),
#endif
  ToolboxMessageTree::Node(I18n::Message::RandomAndApproximation, randomAndApproximationChildren),
  ToolboxMessageTree::Node(I18n::Message::HyperbolicTrigonometry, trigonometryChildren),
  ToolboxMessageTree::Node(I18n::Message::Fluctuation, predictionChildren)};

const ToolboxMessageTree toolboxModel = ToolboxMessageTree::Node(I18n::Message::Toolbox, menu);

MathToolbox::MathToolbox() :
  Toolbox(nullptr, rootModel()->label())
{
}

bool MathToolbox::selectLeaf(int selectedRow) {
  ToolboxMessageTree * messageTree = (ToolboxMessageTree *)m_messageTreeModel->children(selectedRow);
  m_selectableTableView.deselectTable();

  // Translate the message
  const char * text = I18n::translate(messageTree->insertedText());
  char textToInsert[k_maxMessageSize]; // Has to be in the same scope as handleEventWithText
  if (messageTree->label() == messageTree->insertedText()) {
  //  Remove the arguments if we kept one message for both inserted and displayed message
    int maxTextToInsertLength = strlen(text) + 1;
    assert(maxTextToInsertLength <= k_maxMessageSize);
    Shared::ToolboxHelpers::TextToInsertForCommandText(text, textToInsert, maxTextToInsertLength, true);
    text = textToInsert;
  }
  sender()->handleEventWithText(text);
  Container::activeApp()->dismissModalViewController();
  return true;
}

const ToolboxMessageTree * MathToolbox::rootModel() const {
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
