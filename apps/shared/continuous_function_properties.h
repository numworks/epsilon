#ifndef SHARED_CONTINUOUS_FUNCTION_PROPERTIES_H
#define SHARED_CONTINUOUS_FUNCTION_PROPERTIES_H

#include <apps/i18n.h>
#include <ion/bit_helper.h>
#include <poincare/comparison.h>
#include <poincare/conic.h>
#include <poincare/helpers.h>

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

  // === Properties ===
  enum class Status : uint8_t {
    Enabled = 0,
    Undefined,
    Unhandled,
    Banned,
    NumberOfStatus
  };

  constexpr static size_t k_numberOfSymbolTypes = 3;
  // Order impact order of columns in Graph/Values
  enum class SymbolType : uint8_t {
    X = 0,
    Theta,
    T,
    NumberOfSymbolTypes
  };

  enum class CurveParameterType : uint8_t {
    Default,
    CartesianFunction,
    Line,
    HorizontalLine,
    VerticalLine,
    Parametric,
    Polar,
    NumberOfCurveParameterTypes
  };

  constexpr static I18n::Message k_defaultCaption = (I18n::Message)0;
  constexpr static Status k_defaultStatus = Status::Enabled;
  constexpr static Poincare::ComparisonNode::OperatorType k_defaultEquationType = Poincare::ComparisonNode::OperatorType::Equal;
  constexpr static SymbolType k_defaultSymbolType = SymbolType::X;
  constexpr static CurveParameterType k_defaultCurveParameterType = CurveParameterType::Default;
  constexpr static Poincare::Conic::Shape k_defaultConicShape = Poincare::Conic::Shape::Undefined;
  constexpr static bool k_defaultIsOfDegreeTwo = false;
  constexpr static bool k_defaultIsAlongY = false;

  ContinuousFunctionProperties() { reset(); }

  bool isInitialized() const { return m_isInitialized; }

  // Getters
  I18n::Message caption() const { assert(isInitialized()); return (!isEquality() && canBeActive()) ?  I18n::Message::InequalityType : m_caption; }
  Status status() const { assert(isInitialized()); return static_cast<Status>(m_propertiesBitField.m_status); }
  Poincare::ComparisonNode::OperatorType equationType() const { assert(isInitialized()); return static_cast<Poincare::ComparisonNode::OperatorType>(m_propertiesBitField.m_equationType); }
  SymbolType symbolType() const { assert(isInitialized()); return static_cast<SymbolType>(m_propertiesBitField.m_symbolType); }
  CurveParameterType getCurveParameterType() const { assert(isInitialized()); return static_cast<CurveParameterType>(m_propertiesBitField.m_curveParameterType); }
  bool isOfDegreeTwo() const { assert(isInitialized()); return m_propertiesBitField.m_isOfDegreeTwo; }
  Poincare::Conic::Shape conicShape() const { assert(isInitialized()); return static_cast<Poincare::Conic::Shape>(m_propertiesBitField.m_conicShape); }
  bool isAlongY() const { assert(isInitialized()); return m_propertiesBitField.m_isAlongY; }

  // Update
  void reset();
  void update(const Poincare::Expression reducedEquation, const Poincare::Expression inputEquation, Poincare::Context * context, Poincare::ComparisonNode::OperatorType precomputedOperatorType, SymbolType precomputedFunctionSymbol, bool isCartesianEquation);

  // Properties
  bool canBeActive() const { return status() == Status::Enabled; }

  bool isCartesian() const { return symbolType() == SymbolType::X; }
  bool isParametric() const { return symbolType() == SymbolType::T; }
  bool isPolar() const { return symbolType() == SymbolType::Theta; }
  bool isEquality() const { return equationType() == Poincare::ComparisonNode::OperatorType::Equal;}

  bool canBeActiveInTable() const { return !isAlongY() && !isOfDegreeTwo() && isEquality(); }
  bool canHaveCustomDomain() const { return !isAlongY() && isEquality(); }

  bool isLine() const { return getCurveParameterType() == CurveParameterType::VerticalLine || getCurveParameterType() == CurveParameterType::HorizontalLine || getCurveParameterType() == CurveParameterType::Line; }
  bool isConic() const { return conicShape() != Poincare::Conic::Shape::Undefined; }
  bool isCartesianHyperbolaOfDegreeTwo() const { return conicShape() == Poincare::Conic::Shape::Hyperbola && isCartesian() && isOfDegreeTwo(); }

  /* Normalization isn't enforced on Parabola and Hyperbola for a better zooms.
   * It is on Circle and Ellipses so that they don't look like each other. */
  bool enforcePlotNormalization() const { return isPolar() || isParametric() || conicShape() == Poincare::Conic::Shape::Circle || conicShape() == Poincare::Conic::Shape::Ellipse; }

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
  static I18n::Message MessageForSymbolType(SymbolType symbolType);
  I18n::Message symbolMessage() const { return MessageForSymbolType(symbolType()); }

  CodePoint equationSymbol() const { return Poincare::ComparisonNode::ComparisonCodePoint(equationType()); }

private:
  // Update
  void setCartesianFunctionProperties(const Poincare::Expression& analyzedExpression, Poincare::Context * context);
  void setCartesianEquationProperties(const Poincare::Expression& analyzedExpression, Poincare::Context * context, int xDeg, int yDeg, Poincare::TrinaryBoolean highestCoefficientIsPositive);
  void setPolarFunctionProperties(const Poincare::Expression& analyzedExpression, Poincare::Context * context);
  void setParametricFunctionProperties(const Poincare::Expression& analyzedExpression, Poincare::Context * context);

  // If equation has a NonNull coeff. Can also compute last coeff sign.
  static bool HasNonNullCoefficients(const Poincare::Expression equation, const char * symbolName, Poincare::Context * context, Poincare::TrinaryBoolean * highestDegreeCoefficientIsPositive);
  // If equation should be allowed when implicit plots are forbidden.
  static bool IsExplicitEquation(const Poincare::Expression equation, CodePoint symbol);

  // Setters
  void setCaption(I18n::Message caption) { m_caption = caption; }
  void setStatus(Status status) { m_propertiesBitField.m_status = static_cast<uint8_t>(status); }
  void setErrorStatusAndUpdateCaption(Status status);
  void setEquationType(Poincare::ComparisonNode::OperatorType type) { m_propertiesBitField.m_equationType = static_cast<uint8_t>(type); }
  void setSymbolType(SymbolType type) { m_propertiesBitField.m_symbolType = static_cast<uint8_t>(type); }
  void setCurveParameterType(CurveParameterType type) { m_propertiesBitField.m_curveParameterType = static_cast<uint8_t>(type); }
  void setConicShape(Poincare::Conic::Shape shape) { m_propertiesBitField.m_conicShape = static_cast<uint8_t>(shape); }
  void setIsOfDegreeTwo(bool isOfDegreeTwo) { m_propertiesBitField.m_isOfDegreeTwo = isOfDegreeTwo; }
  void setIsAlongY(bool isAlongY) { m_propertiesBitField.m_isAlongY = isAlongY; }

  constexpr static size_t k_numberOfBitsForStatus = Ion::BitHelper::numberOfBitsToRepresentNumberStrictlyInferiorTo(static_cast<unsigned int>(Status::NumberOfStatus));
  constexpr static size_t k_numberOfBitsForEquationType = Ion::BitHelper::numberOfBitsToRepresentNumberStrictlyInferiorTo(static_cast<unsigned int>(Poincare::ComparisonNode::OperatorType::NumberOfTypes));
  constexpr static size_t k_numberOfBitsForSymbolType = Ion::BitHelper::numberOfBitsToRepresentNumberStrictlyInferiorTo(static_cast<unsigned int>(SymbolType::NumberOfSymbolTypes));
  constexpr static size_t k_numberOfBitsForCurveParameterType = Ion::BitHelper::numberOfBitsToRepresentNumberStrictlyInferiorTo(static_cast<unsigned int>(CurveParameterType::NumberOfCurveParameterTypes));
  constexpr static size_t k_numberOfBitsForConicShape = Ion::BitHelper::numberOfBitsToRepresentNumberStrictlyInferiorTo(static_cast<unsigned int>(Poincare::Conic::Shape::NumberOfShapes));

  struct PropertiesBitField { // Current size : 2 bytes
    /* Status */ uint8_t m_status : k_numberOfBitsForStatus;
    /* Poincare::ComparisonNode::OperatorType */ uint8_t m_equationType : k_numberOfBitsForEquationType;
    /* Symbol */ uint8_t m_symbolType : k_numberOfBitsForSymbolType;
    /* CurveParameterType */ uint8_t m_curveParameterType : k_numberOfBitsForCurveParameterType;
    /* Poincare::Conic::Shape */ uint8_t m_conicShape : k_numberOfBitsForConicShape;
    bool m_isOfDegreeTwo : 1;
    bool m_isAlongY : 1;
  };

  PropertiesBitField m_propertiesBitField;
  I18n::Message m_caption;
  bool m_isInitialized;
};

}


#endif
