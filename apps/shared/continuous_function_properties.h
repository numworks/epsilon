#ifndef SHARED_CONTINUOUS_FUNCTION_PROPERTIES_H
#define SHARED_CONTINUOUS_FUNCTION_PROPERTIES_H

#include <apps/i18n.h>
#include <omg/bit_helper.h>
#include <omg/troolean.h>
#include <poincare/expression.h>
#include <poincare/old/comparison.h>
#include <poincare/old/conic.h>
#include <poincare/old/helpers.h>
#include <poincare/old/symbol.h>

/* ContinuousFunctionProperties is an object containing:
 *  - A pointer to a const function type
 *  - An equation type (>, =, <=, etc.)
 * */

namespace Shared {

class ContinuousFunctionProperties {
 public:
  // === Symbols ===
  constexpr static CodePoint k_cartesianSymbol =
      Poincare::Symbol::k_cartesianSymbol;
  constexpr static CodePoint k_parametricSymbol =
      Poincare::Symbol::k_parametricSymbol;
  constexpr static CodePoint k_polarSymbol = Poincare::Symbol::k_polarSymbol;
  constexpr static CodePoint k_radiusSymbol = Poincare::Symbol::k_radiusSymbol;
  constexpr static CodePoint k_ordinateSymbol =
      Poincare::Symbol::k_ordinateSymbol;
  constexpr static char k_ordinateName[2] = {k_ordinateSymbol, '\0'};

  /* Units are not handled when plotting function. The default unit does not
   * matter */
  constexpr static Poincare::Preferences::UnitFormat k_defaultUnitFormat =
      Poincare::Preferences::UnitFormat::Metric;

  // === Properties ===
  enum class Status : uint8_t {
    Enabled = 0,
    Undefined,
    Unhandled,
    Banned,
    LastStatus = Banned,
  };
  /* Use LastStatus instead of the usual NumberOfStatuses to have all values fit
   * on two bits. */
  constexpr static uint8_t k_numberOfStatuses =
      static_cast<uint8_t>(Status::LastStatus) + 1;

  // Order impact order of columns in Graph/Values
  enum class SymbolType : uint8_t {
    X = 0,
    Theta,
    Radius,  // theta=f(r)
    T,
    NoSymbol,
    NumberOfSymbolTypes
  };
  constexpr static size_t k_numberOfSymbolTypes =
      static_cast<size_t>(SymbolType::NumberOfSymbolTypes);
  constexpr static size_t k_numberOfVariableSymbolTypes =
      k_numberOfSymbolTypes - 1;

  enum class CurveParameterType : uint8_t {
    Default,
    CartesianFunction,
    Line,
    HorizontalLine,
    VerticalLine,
    Parametric,
    Polar,
    InversePolar,
    ScatterPlot,
    NumberOfCurveParameterTypes
  };

  constexpr static I18n::Message k_defaultCaption = (I18n::Message)0;
  constexpr static Status k_defaultStatus = Status::Enabled;
  constexpr static Poincare::ComparisonNode::OperatorType
      k_defaultEquationType = Poincare::ComparisonNode::OperatorType::Equal;
  constexpr static SymbolType k_defaultSymbolType = SymbolType::X;
  constexpr static CurveParameterType k_defaultCurveParameterType =
      CurveParameterType::Default;
  constexpr static Poincare::Conic::Shape k_defaultConicShape =
      Poincare::Conic::Shape::Undefined;
  constexpr static bool k_defaultIsOfDegreeTwo = false;
  constexpr static bool k_defaultIsAlongY = false;

  ContinuousFunctionProperties() { reset(); }

  bool isInitialized() const { return m_isInitialized; }

  // Getters
  I18n::Message caption() const {
    assert(m_isInitialized);
    return (!isEquality() && isEnabled()) ? I18n::Message::InequalityType
                                          : m_caption;
  }
  Status status() const {
    assert(m_isInitialized);
    return m_propertiesBitField.m_status;
  }
  Poincare::ComparisonNode::OperatorType equationType() const {
    assert(m_isInitialized);
    return m_propertiesBitField.m_equationType;
  }
  SymbolType symbolType() const {
    assert(m_isInitialized);
    return m_propertiesBitField.m_symbolType;
  }
  CurveParameterType getCurveParameterType() const {
    assert(m_isInitialized);
    return m_propertiesBitField.m_curveParameterType;
  }
  bool isOfDegreeTwo() const {
    assert(m_isInitialized);
    return m_propertiesBitField.m_isOfDegreeTwo;
  }
  Poincare::Conic::Shape conicShape() const {
    assert(m_isInitialized);
    return m_propertiesBitField.m_conicShape;
  }
  bool isAlongY() const {
    assert(m_isInitialized);
    return m_propertiesBitField.m_isAlongY;
  }
  bool hideDetails() const {
    assert(m_isInitialized);
    return m_propertiesBitField.m_hideDetails;
  }

  // Update
  void reset();
  void update(const Poincare::SystemExpression reducedEquation,
              const Poincare::UserExpression inputEquation,
              Poincare::Context* context,
              Poincare::Preferences::ComplexFormat complexFormat,
              Poincare::ComparisonNode::OperatorType precomputedOperatorType,
              SymbolType precomputedFunctionSymbol, bool isCartesianEquation);

  // Properties
  bool isEnabled() const { return status() == Status::Enabled; }

  bool isCartesian() const { return symbolType() == SymbolType::X; }
  bool isParametric() const { return symbolType() == SymbolType::T; }
  bool isPolar() const { return symbolType() == SymbolType::Theta; }
  bool isInversePolar() const { return symbolType() == SymbolType::Radius; }
  bool isEquality() const {
    return equationType() == Poincare::ComparisonNode::OperatorType::Equal;
  }
  bool isEnabledParametric() const { return isEnabled() && isParametric(); }

  bool canBeActiveInTable() const {
    return !isAlongY() && !isOfDegreeTwo() && isEquality() &&
           !isScatterPlot() && isEnabled();
  }
  bool canHaveCustomDomain() const {
    return !isAlongY() && isEquality() && !isScatterPlot();
  }
  bool canDisplayDerivative() const {
    return canBeActiveInTable() && !isInversePolar();
  }
  bool canComputeArea() const {
    return canDisplayDerivative() && isCartesian();
  }
  bool canPlotDerivatives() const {
    return canDisplayDerivative() && isCartesian();
  }
  bool canComputeTangent() const { return canDisplayDerivative(); }

  bool isLine() const {
    return getCurveParameterType() == CurveParameterType::VerticalLine ||
           getCurveParameterType() == CurveParameterType::HorizontalLine ||
           getCurveParameterType() == CurveParameterType::Line;
  }
  bool isConic() const {
    return conicShape() != Poincare::Conic::Shape::Undefined;
  }
  bool isCartesianHyperbolaOfDegreeTwo() const {
    return conicShape() == Poincare::Conic::Shape::Hyperbola && isCartesian() &&
           isOfDegreeTwo();
  }
  bool isScatterPlot() const {
    return getCurveParameterType() == CurveParameterType::ScatterPlot;
  }

  bool canComputeIntersectionsWithFunctionsAlongSameVariable() const {
    return isCartesian() && !isOfDegreeTwo();
  }

  /* Normalization isn't enforced on Parabola and Hyperbola for a better zooms.
   * It is on Circle and Ellipses so that they don't look like each other. */
  bool enforcePlotNormalization() const {
    return isPolar() || isInversePolar() || isParametric() ||
           conicShape() == Poincare::Conic::Shape::Circle ||
           conicShape() == Poincare::Conic::Shape::Ellipse;
  }

  // Wether to draw a dotted or solid line (Strict inequalities).
  bool plotIsDotted() const {
    return equationType() == Poincare::ComparisonNode::OperatorType::Superior ||
           equationType() == Poincare::ComparisonNode::OperatorType::Inferior;
  }

  int numberOfCurveParameters() const { return isParametric() ? 3 : 2; }
  bool parameterAtIndexIsEditable(int index) const;
  bool parameterAtIndexIsPreimage(int index) const;

  CodePoint symbol() const;

  enum class AreaType : uint8_t {
    /* Which area to fill (#) around the curve (|). For example:
     *  Equation:      x^2-1    x^2     x^2+1    x      */
    None = 0,  //  =0    | |      |               |
    Above,     //  >0     -       -        -      |#
    Below,     //  <0     -       -        -     #|
    Inside,    //  <0    |#|      |               -
    Outside    //  >0   #| |#    #|#       #      -
  };

  AreaType areaType() const;
  static I18n::Message MessageForSymbolType(SymbolType symbolType);
  I18n::Message symbolMessage() const {
    return MessageForSymbolType(symbolType());
  }

  const char* equationSymbol() const {
    return Poincare::ComparisonNode::ComparisonOperatorString(equationType());
  }

 private:
  // Update
  I18n::Message captionForCartesianFunction(
      const Poincare::SystemExpression& analyzedExpression);
  void setCartesianEquationProperties(
      const Poincare::SystemExpression& analyzedExpression, int xDeg, int yDeg,
      OMG::Troolean highestCoefficientIsPositive);
  I18n::Message captionForPolarFunction(
      const Poincare::SystemExpression& analyzedExpression);
  I18n::Message captionForParametricFunction(
      const Poincare::SystemExpression& analyzedExpression);

  // If equation should be allowed when implicit plots are forbidden.
  static bool IsExplicitEquation(const Poincare::SystemExpression equation,
                                 CodePoint symbol);

  // Setters
  void setCaption(I18n::Message caption) { m_caption = caption; }
  void setStatus(Status status) { m_propertiesBitField.m_status = status; }
  void setErrorStatusAndUpdateCaption(Status status);
  void setEquationType(Poincare::ComparisonNode::OperatorType type) {
    m_propertiesBitField.m_equationType = type;
  }
  void setSymbolType(SymbolType type) {
    m_propertiesBitField.m_symbolType = type;
  }
  void setCurveParameterType(CurveParameterType type) {
    m_propertiesBitField.m_curveParameterType = type;
  }
  void setConicShape(Poincare::Conic::Shape shape) {
    m_propertiesBitField.m_conicShape = shape;
  }
  void setIsOfDegreeTwo(bool isOfDegreeTwo) {
    m_propertiesBitField.m_isOfDegreeTwo = isOfDegreeTwo;
  }
  void setIsAlongY(bool isAlongY) {
    m_propertiesBitField.m_isAlongY = isAlongY;
  }
  void setHideDetails(bool hideDetails) {
    m_propertiesBitField.m_hideDetails = hideDetails;
  }

  constexpr static size_t k_numberOfBitsForStatus =
      OMG::BitHelper::numberOfBitsToCountUpTo(k_numberOfStatuses);
  constexpr static size_t k_numberOfBitsForEquationType =
      OMG::BitHelper::numberOfBitsToCountUpTo(static_cast<unsigned int>(
          Poincare::ComparisonNode::OperatorType::NumberOfTypes));
  constexpr static size_t k_numberOfBitsForSymbolType =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<unsigned int>(SymbolType::NumberOfSymbolTypes));
  constexpr static size_t k_numberOfBitsForCurveParameterType =
      OMG::BitHelper::numberOfBitsToCountUpTo(static_cast<unsigned int>(
          CurveParameterType::NumberOfCurveParameterTypes));
  constexpr static size_t k_numberOfBitsForConicShape =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<unsigned int>(Poincare::Conic::Shape::NumberOfShapes));

  struct PropertiesBitField {
    Status m_status : k_numberOfBitsForStatus;
    Poincare::ComparisonNode::OperatorType m_equationType
        : k_numberOfBitsForEquationType;
    SymbolType m_symbolType : k_numberOfBitsForSymbolType;
    CurveParameterType m_curveParameterType
        : k_numberOfBitsForCurveParameterType;
    Poincare::Conic::Shape m_conicShape : k_numberOfBitsForConicShape;
    bool m_isOfDegreeTwo : 1;
    bool m_isAlongY : 1;
    bool m_hideDetails : 1;
  };

  I18n::Message m_caption;
  PropertiesBitField m_propertiesBitField;
  bool m_isInitialized;
};

}  // namespace Shared

#endif
