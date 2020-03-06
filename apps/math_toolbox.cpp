#include "math_toolbox.h"
#include "./shared/toolbox_helpers.h"
#include <assert.h>
#include <string.h>

using namespace Poincare;

/* TODO
 * Unit submenu should be created from the Poincare::Unit::Representative table.
 * This would avoid to store duplicates const char *.
 * This would requires classes as:
 * - PointerTree parent of MessageTree and BufferTree
 * - PointerTableCell instead of MessageTableCell
 *
 * We should add in the model tree the possibility to indicate a Leaf that has
 * to be selected when the menu appears. */

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

const ToolboxMessageTree unitTimeSecondChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeSecondSymbol, I18n::Message::UnitTimeSecond),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeSecondMilliSymbol, I18n::Message::UnitTimeSecondMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeSecondMicroSymbol, I18n::Message::UnitTimeSecondMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeSecondNanoSymbol, I18n::Message::UnitTimeSecondNano),
};

const ToolboxMessageTree unitTimeChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitTimeSecondMenu, unitTimeSecondChildren),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeMinuteSymbol, I18n::Message::UnitTimeMinute),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeHourSymbol, I18n::Message::UnitTimeHour),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeDaySymbol, I18n::Message::UnitTimeDay),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeWeekSymbol, I18n::Message::UnitTimeWeek),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeMonthSymbol, I18n::Message::UnitTimeMonth),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeYearSymbol, I18n::Message::UnitTimeYear)};

const ToolboxMessageTree unitDistanceMeterChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterKiloSymbol, I18n::Message::UnitDistanceMeterKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterSymbol, I18n::Message::UnitDistanceMeter),
  ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterMilliSymbol, I18n::Message::UnitDistanceMeterMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterMicroSymbol, I18n::Message::UnitDistanceMeterMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterNanoSymbol, I18n::Message::UnitDistanceMeterNano),
  ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterPicoSymbol, I18n::Message::UnitDistanceMeterPico),
};

const ToolboxMessageTree unitDistanceChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitDistanceMeterMenu, unitDistanceMeterChildren),
  ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceAstronomicalUnitSymbol, I18n::Message::UnitDistanceAstronomicalUnit),
  ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceLightYearSymbol, I18n::Message::UnitDistanceLightYear),
  ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceParsecSymbol, I18n::Message::UnitDistanceParsec)};

const ToolboxMessageTree unitMassChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitMassTonneSymbol, I18n::Message::UnitMassTonne),
  ToolboxMessageTree::Leaf(I18n::Message::UnitMassGramKiloSymbol, I18n::Message::UnitMassGramKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitMassGramSymbol, I18n::Message::UnitMassGram),
  ToolboxMessageTree::Leaf(I18n::Message::UnitMassGramMilliSymbol, I18n::Message::UnitMassGramMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitMassGramMicroSymbol, I18n::Message::UnitMassGramMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitMassGramNanoSymbol, I18n::Message::UnitMassGramNano),
};

const ToolboxMessageTree unitCurrentAmpereChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitCurrentAmpereSymbol, I18n::Message::UnitCurrentAmpere),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCurrentAmpereMilliSymbol, I18n::Message::UnitCurrentAmpereMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCurrentAmpereMicroSymbol, I18n::Message::UnitCurrentAmpereMicro),
};

const ToolboxMessageTree unitTemperatureChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitTemperatureKelvinSymbol, I18n::Message::UnitTemperatureKelvin)};

const ToolboxMessageTree unitAmountMoleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitAmountMoleSymbol, I18n::Message::UnitAmountMole),
  ToolboxMessageTree::Leaf(I18n::Message::UnitAmountMoleMilliSymbol, I18n::Message::UnitAmountMoleMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitAmountMoleMicroSymbol, I18n::Message::UnitAmountMoleMicro),
};

const ToolboxMessageTree unitLuminousIntensityChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitLuminousIntensityCandelaSymbol, I18n::Message::UnitLuminousIntensityCandela)};

const ToolboxMessageTree unitFrequencyHertzChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzGigaSymbol, I18n::Message::UnitFrequencyHertzGiga),
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzMegaSymbol, I18n::Message::UnitFrequencyHertzMega),
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzKiloSymbol, I18n::Message::UnitFrequencyHertzKilo),
ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzSymbol, I18n::Message::UnitFrequencyHertz)};

const ToolboxMessageTree unitForceNewtonChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitForceNewtonKiloSymbol, I18n::Message::UnitForceNewtonKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitForceNewtonSymbol, I18n::Message::UnitForceNewton),
  ToolboxMessageTree::Leaf(I18n::Message::UnitForceNewtonMilliSymbol, I18n::Message::UnitForceNewtonMilli),
};

const ToolboxMessageTree unitPressureChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressurePascalSymbol, I18n::Message::UnitPressurePascal),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressurePascalHectoSymbol, I18n::Message::UnitPressurePascalHecto),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressureBarSymbol, I18n::Message::UnitPressureBar),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressureAtmSymbol, I18n::Message::UnitPressureAtm)};

const ToolboxMessageTree unitEnergyJouleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyJouleKiloSymbol, I18n::Message::UnitEnergyJouleKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyJouleSymbol, I18n::Message::UnitEnergyJoule),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyJouleMilliSymbol, I18n::Message::UnitEnergyJouleMilli),
};

const ToolboxMessageTree unitEnergyElectronVoltChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltMegaSymbol, I18n::Message::UnitEnergyElectronVoltMega),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltKiloSymbol, I18n::Message::UnitEnergyElectronVoltKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltSymbol, I18n::Message::UnitEnergyElectronVolt),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltMilliSymbol, I18n::Message::UnitEnergyElectronVoltMilli),
};

const ToolboxMessageTree unitEnergyChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitEnergyJouleMenu, unitEnergyJouleChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitEnergyEletronVoltMenu, unitEnergyElectronVoltChildren)};

const ToolboxMessageTree unitPowerWattChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattGigaSymbol, I18n::Message::UnitPowerWattGiga),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattMegaSymbol, I18n::Message::UnitPowerWattMega),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattKiloSymbol, I18n::Message::UnitPowerWattKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattSymbol, I18n::Message::UnitPowerWatt),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattMilliSymbol, I18n::Message::UnitPowerWattMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattMicroSymbol, I18n::Message::UnitPowerWattMicro),
};

const ToolboxMessageTree unitElectricChargeCoulombChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitChargeCoulombSymbol, I18n::Message::UnitChargeCoulomb),
};

const ToolboxMessageTree unitPotentialVoltChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltKiloSymbol, I18n::Message::UnitPotentialVoltKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltSymbol, I18n::Message::UnitPotentialVolt),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltMilliSymbol, I18n::Message::UnitPotentialVoltMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltMicroSymbol, I18n::Message::UnitPotentialVoltMicro),
};

const ToolboxMessageTree unitCapacitanceFaradChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitCapacitanceFaradSymbol, I18n::Message::UnitCapacitanceFarad),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCapacitanceFaradMilliSymbol, I18n::Message::UnitCapacitanceFaradMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCapacitanceFaradMicroSymbol, I18n::Message::UnitCapacitanceFaradMicro),
};

const ToolboxMessageTree unitResistanceOhmChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitResistanceOhmKiloSymbol, I18n::Message::UnitResistanceOhmKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitResistanceOhmSymbol, I18n::Message::UnitResistanceOhm),
};

const ToolboxMessageTree unitConductanceSiemensChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitConductanceSiemensSymbol, I18n::Message::UnitConductanceSiemens),
  ToolboxMessageTree::Leaf(I18n::Message::UnitConductanceSiemensMilliSymbol, I18n::Message::UnitConductanceSiemensMilli),
};

const ToolboxMessageTree unitMagneticFieldChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitMagneticFieldTeslaSymbol, I18n::Message::UnitMagneticFieldTesla)};

const ToolboxMessageTree unitInductanceChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitInductanceHenrySymbol, I18n::Message::UnitInductanceHenry)};

const ToolboxMessageTree unitSurfaceChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitSurfaceHectarSymbol, I18n::Message::UnitSurfaceHectar)};

const ToolboxMessageTree unitVolumeLiterChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeLiterSymbol, I18n::Message::UnitVolumeLiter),
  ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeLiterDeciSymbol, I18n::Message::UnitVolumeLiterDeci),
  ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeLiterCentiSymbol, I18n::Message::UnitVolumeLiterCenti),
  ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeLiterMilliSymbol, I18n::Message::UnitVolumeLiterMilli),
};

const ToolboxMessageTree unitChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitTimeMenu, unitTimeChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitDistanceMenu, unitDistanceChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitMassMenu, unitMassChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitCurrentMenu, unitCurrentAmpereChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitTemperatureMenu, unitTemperatureChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitAmountMenu, unitAmountMoleChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitLuminousIntensityMenu, unitLuminousIntensityChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitFrequencyMenu, unitFrequencyHertzChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitForceMenu, unitForceNewtonChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitPressureMenu, unitPressureChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitEnergyMenu, unitEnergyChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitPowerMenu, unitPowerWattChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitElectricChargeMenu, unitElectricChargeCoulombChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitPotentialMenu, unitPotentialVoltChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitCapacitanceMenu, unitCapacitanceFaradChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitResistanceMenu, unitResistanceOhmChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitConductanceMenu, unitConductanceSiemensChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitMagneticFieldMenu, unitMagneticFieldChildren),
  ToolboxMessageTree::Node(I18n::Message::InductanceMenu, unitInductanceChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitSurfaceMenu, unitSurfaceChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitVolumeMenu, unitVolumeLiterChildren),
};

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
  ToolboxMessageTree::Node(I18n::Message::Unit, unitChildren),
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
