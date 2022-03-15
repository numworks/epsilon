#include "unit_comparison_helper.h"
#include "../app.h"
#include <apps/i18n.h>
#include <cmath>
#include <poincare/multiplication.h>
#include <poincare/print_float.h>
#include <poincare/float.h>
#include <poincare/unit.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

namespace UnitComparison {

// If you add new reference value, they always need to be sorted in increasing order.

constexpr static const ReferenceValue k_lengthReferences[] = {
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_timeReferences[] = {
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_massReferences[] = {
  ReferenceValue({I18n::Message::ComputerWeightTitle1, I18n::Message::ComputerWeightTitle2, I18n::Message::ComputerWeightSubtitle, 3.0}),
  ReferenceValue({I18n::Message::HumanWeightTitle1, I18n::Message::HumanWeightTitle2, I18n::Message::HumanWeightSubtitle, 65.0}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_areaReferences[] = {
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_volumeReferences[] = {
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_powerReferences[] = {
  ReferenceValue({I18n::Message::ComputerWeightTitle1, I18n::Message::ComputerWeightTitle2, I18n::Message::ComputerWeightSubtitle, 100.0}),
  ReferenceValue({I18n::Message::HumanWeightTitle1, I18n::Message::HumanWeightTitle2, I18n::Message::HumanWeightSubtitle, 1500.0}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_speedReferences[] = {
  ReferenceValue({I18n::Message::ComputerWeightTitle1, I18n::Message::ComputerWeightTitle2, I18n::Message::ComputerWeightSubtitle, 20.0}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_pressureReferences[] = {
  ReferenceValue({I18n::Message::ComputerWeightTitle1, I18n::Message::ComputerWeightTitle2, I18n::Message::ComputerWeightSubtitle, 20.0}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const int k_numberOfReferenceTables = 8;
constexpr static ReferenceUnit k_referenceUnits[] = {
  ReferenceUnit({"_m", "_m"}), // Length
  ReferenceUnit({"_s", "_s"}), // Time
  ReferenceUnit({"_kg", "_kg"}), // Mass
  ReferenceUnit({"_m^2", "_m^2"}), // Area
  ReferenceUnit({"_m^3", "_m^3"}), // Volume
  ReferenceUnit({"_kg×_m^2×_s^\U00000012-3\U00000013", "_W"}), // Power
  ReferenceUnit({"_m×_s^\U00000012-1\U00000013", "_m×_s^\U00000012-1\U00000013"}), // Speed
  ReferenceUnit({"_kg×_m^\U00000012-1\U00000013×_s^\U00000012-2\U00000013", "_Pa"}) // Pressure
};
constexpr static const ReferenceValue * k_referenceTables[] = {k_lengthReferences, k_timeReferences, k_massReferences, k_areaReferences, k_volumeReferences, k_powerReferences, k_speedReferences, k_pressureReferences};

int SetUpperAndLowerReferenceValues(double inputValue, Expression unit, const ReferenceValue ** referenceValues, int * returnUnitIndex, bool saveComparison) {
  // 1. Find table of corresponding unit.
  const ReferenceValue * valuesOfSameUnit = nullptr;
  char unitBuffer[k_sizeOfUnitBuffer];
  PoincareHelpers::Serialize(unit, unitBuffer, k_sizeOfUnitBuffer);
  int unitIndex = 0;
  while (unitIndex < k_numberOfReferenceTables) {
    if (strncmp(unitBuffer, k_referenceUnits[unitIndex].SIUnit, k_sizeOfUnitBuffer) == 0) {
      valuesOfSameUnit = k_referenceTables[unitIndex];
      break;
    }
    unitIndex++;
  }
  if (valuesOfSameUnit == nullptr) {
    return 0;
  }


  // 2. Iterate through table to find upper and lower values indexes
  int index = 0;
  int upperIndex = -1;
  int lowerIndex = -1;
  while(valuesOfSameUnit[index].value != 0) {
    if (valuesOfSameUnit[index].value >= inputValue) {
      upperIndex = index;
      lowerIndex = index - 1;
      break;
    }
    index ++;
  }
  if (upperIndex < 0) {
    lowerIndex = index - 1;
  }

  // 3. Find ratios and save them if needed
  int indexes[] = {lowerIndex, upperIndex};
  double ratios[] = {0.0, 0.0};
  int nReturn = 0;
  for (int i = 0; i < 2; i++) {
    if (indexes[i] != -1) {
      ratios[i] = inputValue / valuesOfSameUnit[indexes[i]].value;
      if (ratios[i] < 100 && ratios[i] >= 0.01) {
        if (saveComparison) {
          *(referenceValues + nReturn) = &valuesOfSameUnit[indexes[i]];
        }
        nReturn ++;
      }
    }
  }

  if (saveComparison) {
    *returnUnitIndex = unitIndex;
  }

  return nReturn;
}

void FillRatioBuffer(double ratio, char * textBuffer) {
  assert(ratio < 100 && ratio >= 0.01);
  int bufferIndex = 0;
  bool withPercentage = false;
  if (ratio <= 1) {
    ratio = 100.0*ratio;
    withPercentage = true;
  }
  if (ratio >= 99.5) {
    textBuffer[0] = '1';
    textBuffer[1] = '0';
    textBuffer[2] = '0';
    bufferIndex = 3;
  } else {
    bufferIndex = PoincareHelpers::ConvertFloatToText<double>(ratio, textBuffer, k_sizeOfUnitComparisonBuffer - 1, k_numberOfSignicativeNumbers);
  }
  if (withPercentage) {
    textBuffer[bufferIndex] = '%';
    bufferIndex++;
    textBuffer[bufferIndex] = 0;
  }
}

Expression GetComparisonExpression(double value, const ReferenceValue * referenceValue, int unitIndex) {
  assert(unitIndex < k_numberOfReferenceTables);
  double ratio = value / referenceValue->value;
  Expression unit = Poincare::Expression::Parse(k_referenceUnits[unitIndex].displayedUnit, App::app()->localContext());
  return Multiplication::Builder(Float<double>::Builder(ratio), Float<double>::Builder(referenceValue->value), unit);
}

}

}
