#include "continuous_function_properties.h"

using namespace Poincare;

namespace Shared {

ContinuousFunctionProperties::CurveParameter ContinuousFunctionProperties::getCurveParameter(int index) const {
  assert(canBeActive());
  using namespace I18n;
  switch(getCurveParameterType()) {
  case FunctionType::CurveParameterType::CartesianFunction:
    return {index == 0 ? Message::X : Message::Default, true, .isPreimage = index == 1};
  case FunctionType::CurveParameterType::Line:
    return {index == 0 ? Message::X : Message::Y, true, .isPreimage = index == 1};
  case FunctionType::CurveParameterType::HorizontalLine:
    return {index == 0 ? Message::X : Message::Y, index == 0};
  case FunctionType::CurveParameterType::VerticalLine:
    return {index == 0 ? Message::X : Message::Y, index == 1};
  case FunctionType::CurveParameterType::Parametric:
    return {index == 0 ? Message::T : index == 1 ? Message::XOfT : Message::YOfT, index == 0};
  case FunctionType::CurveParameterType::Polar:
    return {index == 0 ? Message::Theta : Message::R, index == 0};
  default:
    // Conics
    return {index == 0 ? Message::X : Message::Y, false};
  }
}

Conic::Type ContinuousFunctionProperties::conicType() const {
  if (!isConic()) {
    return Conic::Type::Unknown;
  }
  if (m_plotType == &k_circleEquation) {
    return Conic::Type::Circle;
  }
  if (m_plotType == &k_ellipseEquation) {
    return Conic::Type::Ellipse;
  }
  if (m_plotType == &k_hyperbolaEquationWithOneSubCurve || m_plotType == &k_hyperbolaEquationWithTwoSubCurves) {
    return Conic::Type::Hyperbola;
  }
  assert(m_plotType == &k_parabolaEquationWithOneSubCurve || m_plotType == &k_parabolaEquationWithTwoSubCurves);
  return Conic::Type::Parabola;
}

ContinuousFunctionProperties::AreaType ContinuousFunctionProperties::areaType() const {
  assert(isInitialized());
  if (!canBeActive() || m_equationType == ComparisonNode::OperatorType::Equal) {
    return AreaType::None;
  }
  // To draw y^2>a, the area plotted should be Outside and not Above.
  if (m_equationType == ComparisonNode::OperatorType::Inferior || m_equationType == ComparisonNode::OperatorType::InferiorEqual) {
    return numberOfSubCurves() == 1 ? AreaType::Below : AreaType::Inside;
  }
  assert(m_equationType == ComparisonNode::OperatorType::Superior || m_equationType == ComparisonNode::OperatorType::SuperiorEqual);
  return numberOfSubCurves() == 1 ? AreaType::Above : AreaType::Outside;
}

CodePoint ContinuousFunctionProperties::symbol() const {
  switch (symbolType()) {
  case FunctionType::SymbolType::T:
    return k_parametricSymbol;
  case FunctionType::SymbolType::Theta:
    return k_polarSymbol;
  default:
    assert(symbolType() == FunctionType::SymbolType::X);
    return k_cartesianSymbol;
  }
}

I18n::Message ContinuousFunctionProperties::MessageForSymbolType(FunctionType::SymbolType symbolType) {
  switch (symbolType) {
  case FunctionType::SymbolType::T:
    return I18n::Message::T;
  case FunctionType::SymbolType::Theta:
    return I18n::Message::Theta;
  default:
    assert(symbolType == FunctionType::SymbolType::X);
    return I18n::Message::X;
  }
}

}
