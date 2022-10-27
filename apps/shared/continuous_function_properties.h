#ifndef SHARED_CONTINUOUS_FUNCTION_PROPERTIES_H
#define SHARED_CONTINUOUS_FUNCTION_PROPERTIES_H

#include "continuous_function_types.h"
#include <apps/i18n.h>
#include <poincare/comparison.h>
#include <poincare/conic.h>

/* ContinuousFunctionProperties is an object containing:
 *  - A pointer to a const function type
 *  - An equation type (>, =, <=, etc.)
 * */

namespace Shared {

class ContinuousFunctionProperties {
public:
  // === Symbols ===
  constexpr static CodePoint k_cartesianSymbol = 'x';
  constexpr static CodePoint k_parametricSymbol = 't';
  constexpr static CodePoint k_polarSymbol = UCodePointGreekSmallLetterTheta;
  constexpr static CodePoint k_radiusSymbol = 'r';
  constexpr static CodePoint k_ordinateSymbol = 'y';
  constexpr static char k_ordinateName[2] = "y";
  static_assert(k_ordinateSymbol == k_ordinateName[0]);

  // Units are not handled when plotting function. The default unit does not matters
  constexpr static Poincare::Preferences::UnitFormat k_defaultUnitFormat = Poincare::Preferences::UnitFormat::Metric;

  ContinuousFunctionProperties() :
    m_plotType(&FunctionTypes::k_uninitializedFunctionType),
    m_equationType(Poincare::ComparisonNode::OperatorType::Equal)
  {}

  // Getters
  I18n::Message caption() const { return isEquality() ? m_plotType->caption() : I18n::Message::InequalityType;  }
  bool isInitialized() const { return m_plotType->isInitialized(); }
  FunctionType::Status status() const { return m_plotType->status(); }
  FunctionType::SymbolType symbolType() const { return m_plotType->symbolType(); }
  int numberOfSubCurves() const { return m_plotType->numberOfSubCurves(); }
  bool isAlongY() const { return m_plotType->isAlongY(); }
  Poincare::Conic::Shape conicShape() const { return m_plotType->conicShape(); }
  bool isLine() const { return m_plotType->isLine(); }
  FunctionType::CurveParameterType getCurveParameterType() const { return m_plotType->getCurveParameterType(); }

  // Properties
  bool canBeActive() const { return status() == FunctionType::Status::Enabled; }

  bool isCartesian() const { return symbolType() == FunctionType::SymbolType::X; }
  bool isParametric() const { return symbolType() == FunctionType::SymbolType::T; }
  bool isPolar() const { return symbolType() == FunctionType::SymbolType::Theta; }
  bool isEquality() const { return equationType() == Poincare::ComparisonNode::OperatorType::Equal;}

  bool canBeActiveInTable() const { return !isAlongY() && numberOfSubCurves() == 1 && isEquality(); }
  bool canHaveCustomDomain() const { return !isAlongY() && isEquality(); }

  bool isConic() const { return conicShape() != Poincare::Conic::Shape::Undefined; }
  bool isNotCartesianParabolaOrHyperbola() const { return !isCartesian() || (conicShape() != Poincare::Conic::Shape::Parabola && conicShape() != Poincare::Conic::Shape::Hyperbola); }
  bool isHyperbolaWithTwoSubCurves() const { return m_plotType == &FunctionTypes::k_hyperbolaEquationWithTwoSubCurves; }

  // Wether to draw a dotted or solid line (Strict inequalities).
  bool plotIsDotted() const { return equationType() == Poincare::ComparisonNode::OperatorType::Superior ||  equationType() == Poincare::ComparisonNode::OperatorType::Inferior;}

  struct CurveParameter {
    I18n::Message parameterName;
    bool editable;
    bool isPreimage;
  };
  int numberOfCurveParameters() const { return isParametric() ? 3 : 2; }
  CurveParameter getCurveParameter(int index) const;

  CodePoint symbol() const;

  enum class AreaType : uint8_t {
    /* Which area to fill (#) around the curve (|). For example:
     *  Equation:      x^2-1    x^2     x^2+1    x      */
    None = 0, //  =0    | |      |               |
    Above,    //  >0     -       -        -      |#
    Below,    //  <0     -       -        -     #|
    Inside,   //  <0    |#|      |               -
    Outside   //  >0   #| |#    #|#       #      -
  };

  AreaType areaType() const;

  static I18n::Message MessageForSymbolType(FunctionType::SymbolType symbolType);
  I18n::Message symbolMessage() const { return MessageForSymbolType(symbolType()); }

  // Equation type
  Poincare::ComparisonNode::OperatorType equationType() const { assert(isInitialized()); return m_equationType; }
  CodePoint equationSymbol() const { return Poincare::ComparisonNode::ComparisonCodePoint(equationType()); }

  // Update
  void update(const Poincare::Expression reducedEquation, const Poincare::Expression inputEquation, Poincare::Context * context, Poincare::ComparisonNode::OperatorType precomputedOperatorType, FunctionType::SymbolType precomputedFunctionSymbol);
  void reset() { m_plotType = &FunctionTypes::k_uninitializedFunctionType; }

private:
  static const FunctionType * CartesianFunctionAnalysis(const Poincare::Expression& reducedEquation, Poincare::Context * context);
  static const FunctionType * CartesianEquationAnalysis(const Poincare::Expression& reducedEquation, Poincare::Context * context, int xDeg, int yDeg, Poincare::TrinaryBoolean highestCoefficientIsPositive);
  static const FunctionType * PolarFunctionAnalysis(const Poincare::Expression& reducedEquation, Poincare::Context * context);
  static const FunctionType * ParametricFunctionAnalysis(const Poincare::Expression& reducedEquation, Poincare::Context * context);

  // If equation has a NonNull coeff. Can also compute last coeff sign.
  static bool HasNonNullCoefficients(const Poincare::Expression equation, const char * symbolName, Poincare::Context * context, Poincare::TrinaryBoolean * highestDegreeCoefficientIsPositive);
  // If equation should be allowed when implicit plots are forbidden.
  static bool IsExplicitEquation(const Poincare::Expression equation, CodePoint symbol);

  void setEquationType(Poincare::ComparisonNode::OperatorType equationType) { m_equationType = equationType; }
  void setFunctionType(const FunctionType * plotType) { m_plotType = plotType; }

  const FunctionType * m_plotType;
  Poincare::ComparisonNode::OperatorType m_equationType;
};

}


#endif
