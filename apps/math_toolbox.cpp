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

const ToolboxMessageTree probabilityChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::BinomialCommandWithArg, I18n::Message::Combination),
  ToolboxMessageTree::Leaf(I18n::Message::PermuteCommandWithArg, I18n::Message::Permutation)
};

const ToolboxMessageTree arithmeticChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::GcdCommandWithArg, I18n::Message::GreatCommonDivisor),
  ToolboxMessageTree::Leaf(I18n::Message::LcmCommandWithArg, I18n::Message::LeastCommonMultiple),
  ToolboxMessageTree::Leaf(I18n::Message::FactorCommandWithArg, I18n::Message::PrimeFactorDecomposition),
  ToolboxMessageTree::Leaf(I18n::Message::RemCommandWithArg, I18n::Message::Remainder),
  ToolboxMessageTree::Leaf(I18n::Message::QuoCommandWithArg, I18n::Message::Quotient)
};

#if MATRICES_ARE_DEFINED
const ToolboxMessageTree matricesChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::MatrixCommandWithArg, I18n::Message::NewMatrix, false, I18n::Message::MatrixCommand),
  ToolboxMessageTree::Leaf(I18n::Message::IndentityCommandWithArg, I18n::Message::Identity),
  ToolboxMessageTree::Leaf(I18n::Message::InverseCommandWithArg, I18n::Message::Inverse),
  ToolboxMessageTree::Leaf(I18n::Message::DeterminantCommandWithArg, I18n::Message::Determinant),
  ToolboxMessageTree::Leaf(I18n::Message::TransposeCommandWithArg, I18n::Message::Transpose),
  ToolboxMessageTree::Leaf(I18n::Message::TraceCommandWithArg, I18n::Message::Trace),
  ToolboxMessageTree::Leaf(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension)
};
#endif

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

const ToolboxMessageTree chemistryMolarMassesByNumber[] = {
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementH, I18n::Message::ElementHMass, false, I18n::Message::ElementHMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementHe, I18n::Message::ElementHeMass, false, I18n::Message::ElementHeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementLi, I18n::Message::ElementLiMass, false, I18n::Message::ElementLiMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBe, I18n::Message::ElementBeMass, false, I18n::Message::ElementBeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementB, I18n::Message::ElementBMass, false, I18n::Message::ElementBMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementC, I18n::Message::ElementCMass, false, I18n::Message::ElementCMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementN, I18n::Message::ElementNMass, false, I18n::Message::ElementNMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementO, I18n::Message::ElementOMass, false, I18n::Message::ElementOMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementF, I18n::Message::ElementFMass, false, I18n::Message::ElementFMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNe, I18n::Message::ElementNeMass, false, I18n::Message::ElementNeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNa, I18n::Message::ElementNaMass, false, I18n::Message::ElementNaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMg, I18n::Message::ElementMgMass, false, I18n::Message::ElementMgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAl, I18n::Message::ElementAlMass, false, I18n::Message::ElementAlMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSi, I18n::Message::ElementSiMass, false, I18n::Message::ElementSiMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementP, I18n::Message::ElementPMass, false, I18n::Message::ElementPMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementS, I18n::Message::ElementSMass, false, I18n::Message::ElementSMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCl, I18n::Message::ElementClMass, false, I18n::Message::ElementClMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAr, I18n::Message::ElementArMass, false, I18n::Message::ElementArMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementK, I18n::Message::ElementKMass, false, I18n::Message::ElementKMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCa, I18n::Message::ElementCaMass, false, I18n::Message::ElementCaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSc, I18n::Message::ElementScMass, false, I18n::Message::ElementScMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTi, I18n::Message::ElementTiMass, false, I18n::Message::ElementTiMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementV, I18n::Message::ElementVMass, false, I18n::Message::ElementVMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCr, I18n::Message::ElementCrMass, false, I18n::Message::ElementCrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMn, I18n::Message::ElementMnMass, false, I18n::Message::ElementMnMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementFe, I18n::Message::ElementFeMass, false, I18n::Message::ElementFeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCo, I18n::Message::ElementCoMass, false, I18n::Message::ElementCoMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNi, I18n::Message::ElementNiMass, false, I18n::Message::ElementNiMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCu, I18n::Message::ElementCuMass, false, I18n::Message::ElementCuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementZn, I18n::Message::ElementZnMass, false, I18n::Message::ElementZnMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementGa, I18n::Message::ElementGaMass, false, I18n::Message::ElementGaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementGe, I18n::Message::ElementGeMass, false, I18n::Message::ElementGeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAs, I18n::Message::ElementAsMass, false, I18n::Message::ElementAsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSe, I18n::Message::ElementSeMass, false, I18n::Message::ElementSeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBr, I18n::Message::ElementBrMass, false, I18n::Message::ElementBrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementKr, I18n::Message::ElementKrMass, false, I18n::Message::ElementKrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRb, I18n::Message::ElementRbMass, false, I18n::Message::ElementRbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSr, I18n::Message::ElementSrMass, false, I18n::Message::ElementSrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementY, I18n::Message::ElementYMass, false, I18n::Message::ElementYMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementZr, I18n::Message::ElementZrMass, false, I18n::Message::ElementZrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNb, I18n::Message::ElementNbMass, false, I18n::Message::ElementNbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMo, I18n::Message::ElementMoMass, false, I18n::Message::ElementMoMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTc, I18n::Message::ElementTcMass, false, I18n::Message::ElementTcMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRu, I18n::Message::ElementRuMass, false, I18n::Message::ElementRuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRh, I18n::Message::ElementRhMass, false, I18n::Message::ElementRhMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPd, I18n::Message::ElementPdMass, false, I18n::Message::ElementPdMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAg, I18n::Message::ElementAgMass, false, I18n::Message::ElementAgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCd, I18n::Message::ElementCdMass, false, I18n::Message::ElementCdMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementIn, I18n::Message::ElementInMass, false, I18n::Message::ElementInMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSn, I18n::Message::ElementSnMass, false, I18n::Message::ElementSnMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSb, I18n::Message::ElementSbMass, false, I18n::Message::ElementSbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTe, I18n::Message::ElementTeMass, false, I18n::Message::ElementTeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementI, I18n::Message::ElementIMass, false, I18n::Message::ElementIMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementXe, I18n::Message::ElementXeMass, false, I18n::Message::ElementXeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCs, I18n::Message::ElementCsMass, false, I18n::Message::ElementCsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBa, I18n::Message::ElementBaMass, false, I18n::Message::ElementBaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementLa, I18n::Message::ElementLaMass, false, I18n::Message::ElementLaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCe, I18n::Message::ElementCeMass, false, I18n::Message::ElementCeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPr, I18n::Message::ElementPrMass, false, I18n::Message::ElementPrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNd, I18n::Message::ElementNdMass, false, I18n::Message::ElementNdMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPm, I18n::Message::ElementPmMass, false, I18n::Message::ElementPmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSm, I18n::Message::ElementSmMass, false, I18n::Message::ElementSmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementEu, I18n::Message::ElementEuMass, false, I18n::Message::ElementEuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementGd, I18n::Message::ElementGdMass, false, I18n::Message::ElementGdMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTb, I18n::Message::ElementTbMass, false, I18n::Message::ElementTbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementDy, I18n::Message::ElementDyMass, false, I18n::Message::ElementDyMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementHo, I18n::Message::ElementHoMass, false, I18n::Message::ElementHoMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementEr, I18n::Message::ElementErMass, false, I18n::Message::ElementErMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTm, I18n::Message::ElementTmMass, false, I18n::Message::ElementTmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementYb, I18n::Message::ElementYbMass, false, I18n::Message::ElementYbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementLu, I18n::Message::ElementLuMass, false, I18n::Message::ElementLuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementHf, I18n::Message::ElementHfMass, false, I18n::Message::ElementHfMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTa, I18n::Message::ElementTaMass, false, I18n::Message::ElementTaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementW, I18n::Message::ElementWMass, false, I18n::Message::ElementWMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRe, I18n::Message::ElementReMass, false, I18n::Message::ElementReMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementOs, I18n::Message::ElementOsMass, false, I18n::Message::ElementOsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementIr, I18n::Message::ElementIrMass, false, I18n::Message::ElementIrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPt, I18n::Message::ElementPtMass, false, I18n::Message::ElementPtMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAu, I18n::Message::ElementAuMass, false, I18n::Message::ElementAuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementHg, I18n::Message::ElementHgMass, false, I18n::Message::ElementHgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTl, I18n::Message::ElementTlMass, false, I18n::Message::ElementTlMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPb, I18n::Message::ElementPbMass, false, I18n::Message::ElementPbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBi, I18n::Message::ElementBiMass, false, I18n::Message::ElementBiMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPo, I18n::Message::ElementPoMass, false, I18n::Message::ElementPoMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAt, I18n::Message::ElementAtMass, false, I18n::Message::ElementAtMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRn, I18n::Message::ElementRnMass, false, I18n::Message::ElementRnMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementFr, I18n::Message::ElementFrMass, false, I18n::Message::ElementFrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRa, I18n::Message::ElementRaMass, false, I18n::Message::ElementRaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAc, I18n::Message::ElementAcMass, false, I18n::Message::ElementAcMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTh, I18n::Message::ElementThMass, false, I18n::Message::ElementThMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPa, I18n::Message::ElementPaMass, false, I18n::Message::ElementPaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementU, I18n::Message::ElementUMass, false, I18n::Message::ElementUMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNp, I18n::Message::ElementNpMass, false, I18n::Message::ElementNpMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPu, I18n::Message::ElementPuMass, false, I18n::Message::ElementPuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAm, I18n::Message::ElementAmMass, false, I18n::Message::ElementAmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCm, I18n::Message::ElementCmMass, false, I18n::Message::ElementCmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBk, I18n::Message::ElementBkMass, false, I18n::Message::ElementBkMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCf, I18n::Message::ElementCfMass, false, I18n::Message::ElementCfMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementEs, I18n::Message::ElementEsMass, false, I18n::Message::ElementEsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementFm, I18n::Message::ElementFmMass, false, I18n::Message::ElementFmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMd, I18n::Message::ElementMdMass, false, I18n::Message::ElementMdMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNo, I18n::Message::ElementNoMass, false, I18n::Message::ElementNoMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementLr, I18n::Message::ElementLrMass, false, I18n::Message::ElementLrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRf, I18n::Message::ElementRfMass, false, I18n::Message::ElementRfMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementDb, I18n::Message::ElementDbMass, false, I18n::Message::ElementDbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSg, I18n::Message::ElementSgMass, false, I18n::Message::ElementSgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBh, I18n::Message::ElementBhMass, false, I18n::Message::ElementBhMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementHs, I18n::Message::ElementHsMass, false, I18n::Message::ElementHsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMt, I18n::Message::ElementMtMass, false, I18n::Message::ElementMtMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementDs, I18n::Message::ElementDsMass, false, I18n::Message::ElementDsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRg, I18n::Message::ElementRgMass, false, I18n::Message::ElementRgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCn, I18n::Message::ElementCnMass, false, I18n::Message::ElementCnMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNh, I18n::Message::ElementNhMass, false, I18n::Message::ElementNhMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementFl, I18n::Message::ElementFlMass, false, I18n::Message::ElementFlMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMc, I18n::Message::ElementMcMass, false, I18n::Message::ElementMcMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementLv, I18n::Message::ElementLvMass, false, I18n::Message::ElementLvMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTs, I18n::Message::ElementTsMass, false, I18n::Message::ElementTsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementOg, I18n::Message::ElementOgMass, false, I18n::Message::ElementOgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementUue, I18n::Message::ElementUueMass, false, I18n::Message::ElementUueMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementUbn, I18n::Message::ElementUbnMass, false, I18n::Message::ElementUbnMass)
};

const ToolboxMessageTree chemistryMolarMassesByAlpha[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAc, I18n::Message::ElementAcMass, false, I18n::Message::ElementAcMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAg, I18n::Message::ElementAgMass, false, I18n::Message::ElementAgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAl, I18n::Message::ElementAlMass, false, I18n::Message::ElementAlMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAm, I18n::Message::ElementAmMass, false, I18n::Message::ElementAmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAr, I18n::Message::ElementArMass, false, I18n::Message::ElementArMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAs, I18n::Message::ElementAsMass, false, I18n::Message::ElementAsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAt, I18n::Message::ElementAtMass, false, I18n::Message::ElementAtMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAu, I18n::Message::ElementAuMass, false, I18n::Message::ElementAuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementB, I18n::Message::ElementBMass, false, I18n::Message::ElementBMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBa, I18n::Message::ElementBaMass, false, I18n::Message::ElementBaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBe, I18n::Message::ElementBeMass, false, I18n::Message::ElementBeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBh, I18n::Message::ElementBhMass, false, I18n::Message::ElementBhMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBi, I18n::Message::ElementBiMass, false, I18n::Message::ElementBiMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBk, I18n::Message::ElementBkMass, false, I18n::Message::ElementBkMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBr, I18n::Message::ElementBrMass, false, I18n::Message::ElementBrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementC, I18n::Message::ElementCMass, false, I18n::Message::ElementCMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCa, I18n::Message::ElementCaMass, false, I18n::Message::ElementCaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCd, I18n::Message::ElementCdMass, false, I18n::Message::ElementCdMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCe, I18n::Message::ElementCeMass, false, I18n::Message::ElementCeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCf, I18n::Message::ElementCfMass, false, I18n::Message::ElementCfMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCl, I18n::Message::ElementClMass, false, I18n::Message::ElementClMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCm, I18n::Message::ElementCmMass, false, I18n::Message::ElementCmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCn, I18n::Message::ElementCnMass, false, I18n::Message::ElementCnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCo, I18n::Message::ElementCoMass, false, I18n::Message::ElementCoMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCs, I18n::Message::ElementCsMass, false, I18n::Message::ElementCsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCr, I18n::Message::ElementCrMass, false, I18n::Message::ElementCrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCu, I18n::Message::ElementCuMass, false, I18n::Message::ElementCuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementDb, I18n::Message::ElementDbMass, false, I18n::Message::ElementDbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementDs, I18n::Message::ElementDsMass, false, I18n::Message::ElementDsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementDy, I18n::Message::ElementDyMass, false, I18n::Message::ElementDyMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementEr, I18n::Message::ElementErMass, false, I18n::Message::ElementErMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementEs, I18n::Message::ElementEsMass, false, I18n::Message::ElementEsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementEu, I18n::Message::ElementEuMass, false, I18n::Message::ElementEuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementF, I18n::Message::ElementFMass, false, I18n::Message::ElementFMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementFe, I18n::Message::ElementFeMass, false, I18n::Message::ElementFeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementFl, I18n::Message::ElementFlMass, false, I18n::Message::ElementFlMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementFm, I18n::Message::ElementFmMass, false, I18n::Message::ElementFmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementFr, I18n::Message::ElementFrMass, false, I18n::Message::ElementFrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementGa, I18n::Message::ElementGaMass, false, I18n::Message::ElementGaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementGd, I18n::Message::ElementGdMass, false, I18n::Message::ElementGdMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementGe, I18n::Message::ElementGeMass, false, I18n::Message::ElementGeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementH, I18n::Message::ElementHMass, false, I18n::Message::ElementHMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementHe, I18n::Message::ElementHeMass, false, I18n::Message::ElementHeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementHf, I18n::Message::ElementHfMass, false, I18n::Message::ElementHfMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementHg, I18n::Message::ElementHgMass, false, I18n::Message::ElementHgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementHo, I18n::Message::ElementHoMass, false, I18n::Message::ElementHoMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementHs, I18n::Message::ElementHsMass, false, I18n::Message::ElementHsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementI, I18n::Message::ElementIMass, false, I18n::Message::ElementIMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementIn, I18n::Message::ElementInMass, false, I18n::Message::ElementInMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementIr, I18n::Message::ElementIrMass, false, I18n::Message::ElementIrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementK, I18n::Message::ElementKMass, false, I18n::Message::ElementKMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementKr, I18n::Message::ElementKrMass, false, I18n::Message::ElementKrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementLa, I18n::Message::ElementLaMass, false, I18n::Message::ElementLaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementLi, I18n::Message::ElementLiMass, false, I18n::Message::ElementLiMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementLr, I18n::Message::ElementLrMass, false, I18n::Message::ElementLrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementLu, I18n::Message::ElementLuMass, false, I18n::Message::ElementLuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementLv, I18n::Message::ElementLvMass, false, I18n::Message::ElementLvMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMc, I18n::Message::ElementMcMass, false, I18n::Message::ElementMcMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMd, I18n::Message::ElementMdMass, false, I18n::Message::ElementMdMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMg, I18n::Message::ElementMgMass, false, I18n::Message::ElementMgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMn, I18n::Message::ElementMnMass, false, I18n::Message::ElementMnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMo, I18n::Message::ElementMoMass, false, I18n::Message::ElementMoMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMt, I18n::Message::ElementMtMass, false, I18n::Message::ElementMtMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementN, I18n::Message::ElementNMass, false, I18n::Message::ElementNMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNa, I18n::Message::ElementNaMass, false, I18n::Message::ElementNaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNb, I18n::Message::ElementNbMass, false, I18n::Message::ElementNbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNd, I18n::Message::ElementNdMass, false, I18n::Message::ElementNdMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNe, I18n::Message::ElementNeMass, false, I18n::Message::ElementNeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNh, I18n::Message::ElementNhMass, false, I18n::Message::ElementNhMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNi, I18n::Message::ElementNiMass, false, I18n::Message::ElementNiMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNo, I18n::Message::ElementNoMass, false, I18n::Message::ElementNoMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNp, I18n::Message::ElementNpMass, false, I18n::Message::ElementNpMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementO, I18n::Message::ElementOMass, false, I18n::Message::ElementOMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementOg, I18n::Message::ElementOgMass, false, I18n::Message::ElementOgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementOs, I18n::Message::ElementOsMass, false, I18n::Message::ElementOsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementP, I18n::Message::ElementPMass, false, I18n::Message::ElementPMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPa, I18n::Message::ElementPaMass, false, I18n::Message::ElementPaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPb, I18n::Message::ElementPbMass, false, I18n::Message::ElementPbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPd, I18n::Message::ElementPdMass, false, I18n::Message::ElementPdMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPm, I18n::Message::ElementPmMass, false, I18n::Message::ElementPmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPo, I18n::Message::ElementPoMass, false, I18n::Message::ElementPoMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPr, I18n::Message::ElementPrMass, false, I18n::Message::ElementPrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPt, I18n::Message::ElementPtMass, false, I18n::Message::ElementPtMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPu, I18n::Message::ElementPuMass, false, I18n::Message::ElementPuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRa, I18n::Message::ElementRaMass, false, I18n::Message::ElementRaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRb, I18n::Message::ElementRbMass, false, I18n::Message::ElementRbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRe, I18n::Message::ElementReMass, false, I18n::Message::ElementReMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRf, I18n::Message::ElementRfMass, false, I18n::Message::ElementRfMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRg, I18n::Message::ElementRgMass, false, I18n::Message::ElementRgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRh, I18n::Message::ElementRhMass, false, I18n::Message::ElementRhMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRn, I18n::Message::ElementRnMass, false, I18n::Message::ElementRnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRu, I18n::Message::ElementRuMass, false, I18n::Message::ElementRuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementS, I18n::Message::ElementSMass, false, I18n::Message::ElementSMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSb, I18n::Message::ElementSbMass, false, I18n::Message::ElementSbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSc, I18n::Message::ElementScMass, false, I18n::Message::ElementScMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSe, I18n::Message::ElementSeMass, false, I18n::Message::ElementSeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSg, I18n::Message::ElementSgMass, false, I18n::Message::ElementSgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSi, I18n::Message::ElementSiMass, false, I18n::Message::ElementSiMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSm, I18n::Message::ElementSmMass, false, I18n::Message::ElementSmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSn, I18n::Message::ElementSnMass, false, I18n::Message::ElementSnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSr, I18n::Message::ElementSrMass, false, I18n::Message::ElementSrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTa, I18n::Message::ElementTaMass, false, I18n::Message::ElementTaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTb, I18n::Message::ElementTbMass, false, I18n::Message::ElementTbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTc, I18n::Message::ElementTcMass, false, I18n::Message::ElementTcMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTe, I18n::Message::ElementTeMass, false, I18n::Message::ElementTeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTh, I18n::Message::ElementThMass, false, I18n::Message::ElementThMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTi, I18n::Message::ElementTiMass, false, I18n::Message::ElementTiMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTl, I18n::Message::ElementTlMass, false, I18n::Message::ElementTlMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTm, I18n::Message::ElementTmMass, false, I18n::Message::ElementTmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTs, I18n::Message::ElementTsMass, false, I18n::Message::ElementTsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementU, I18n::Message::ElementUMass, false, I18n::Message::ElementUMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementUue, I18n::Message::ElementUueMass, false, I18n::Message::ElementUueMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementUbn, I18n::Message::ElementUbnMass, false, I18n::Message::ElementUbnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementV, I18n::Message::ElementVMass, false, I18n::Message::ElementVMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementW, I18n::Message::ElementWMass, false, I18n::Message::ElementWMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementXe, I18n::Message::ElementXeMass, false, I18n::Message::ElementXeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementY, I18n::Message::ElementYMass, false, I18n::Message::ElementYMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementYb, I18n::Message::ElementYbMass, false, I18n::Message::ElementYbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementZn, I18n::Message::ElementZnMass, false, I18n::Message::ElementZnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementZr, I18n::Message::ElementZrMass, false, I18n::Message::ElementZrMass)
};

const ToolboxMessageTree chemistry[] = {
  ToolboxMessageTree::Node(I18n::Message::MolarMassesByNumber, chemistryMolarMassesByNumber),
  ToolboxMessageTree::Node(I18n::Message::MolarMassesByAlpha, chemistryMolarMassesByAlpha)
};

const ToolboxMessageTree physics[] = {
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementH, I18n::Message::ElementHMass, false, I18n::Message::ElementHMass)
};

const ToolboxMessageTree menu[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue),
  ToolboxMessageTree::Leaf(I18n::Message::RootCommandWithArg, I18n::Message::NthRoot),
  ToolboxMessageTree::Leaf(I18n::Message::LogCommandWithArg, I18n::Message::BasedLogarithm),
  ToolboxMessageTree::Node(I18n::Message::Calculation, calculChildren),
  ToolboxMessageTree::Node(I18n::Message::ComplexNumber, complexChildren),
  ToolboxMessageTree::Node(I18n::Message::Probability, probabilityChildren),
  ToolboxMessageTree::Node(I18n::Message::Arithmetic, arithmeticChildren),
#if MATRICES_ARE_DEFINED
  ToolboxMessageTree::Node(I18n::Message::Matrices, matricesChildren),
#endif
#if LIST_ARE_DEFINED
  ToolboxMessageTree::Node(I18n::Message::Lists,listsChildren),
#endif
  ToolboxMessageTree::Node(I18n::Message::RandomAndApproximation, randomAndApproximationChildren),
  ToolboxMessageTree::Node(I18n::Message::HyperbolicTrigonometry, trigonometryChildren),
  ToolboxMessageTree::Node(I18n::Message::Fluctuation, predictionChildren),
  ToolboxMessageTree::Node(I18n::Message::Chemistry, chemistry)};

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
