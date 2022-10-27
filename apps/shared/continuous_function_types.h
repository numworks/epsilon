#ifndef SHARED_CONTINUOUS_FUNCTION_TYPES_H
#define SHARED_CONTINUOUS_FUNCTION_TYPES_H

#include <apps/i18n.h>
#include <poincare/conic.h>

/* A function type is a constexpr object that contains datas about a specific
 * type of function. All these constexpr are declared at the end of this file.
 * */

namespace Shared {

class FunctionType {
public:
  enum class Status : uint8_t {
    Enabled = 0,
    Undefined,
    Unhandled,
    Banned
  };

  constexpr static size_t k_numberOfSymbolTypes = 3;
  // Order impact order of columns in Graph/Values
  enum class SymbolType : uint8_t {
    X = 0,
    Theta,
    T,
    Unknown
  };

  enum class CurveParameterType : uint8_t {
    Default,
    CartesianFunction,
    Line,
    HorizontalLine,
    VerticalLine,
    Parametric,
    Polar
  };

  constexpr FunctionType(I18n::Message caption) : m_caption(caption) {}

  virtual bool isInitialized() const { return true; }
  I18n::Message caption() const { assert(isInitialized()); return m_caption; }
  virtual Status status() const { assert(isInitialized()); return Status::Enabled; }
  virtual SymbolType symbolType() const { assert(isInitialized()); return SymbolType::Unknown; }
  virtual int numberOfSubCurves() const { assert(isInitialized()); return 1; }
  virtual Poincare::Conic::Shape conicShape() const { assert(isInitialized()); return Poincare::Conic::Shape::Undefined; }
  virtual bool isAlongY() const { assert(isInitialized()); return false; }
  virtual bool isLine() const { assert(isInitialized()); return false; }
  virtual CurveParameterType getCurveParameterType() const { assert(isInitialized()); return CurveParameterType::Default; }

private:
  /* TODO:
   * There are a lot of partially duplicated captions, like
   * "Polar equation of an ellipse" which shares half of its caption with
   * "Parametric equation of an ellipse" and the other half with
   * "Polar equation of a circle".
   * Only "Polar equation %s", "Parametric equation %s", "of a circle" and
   * "of an ellipse" could be stored. */
  const I18n::Message m_caption;
};

class UninitializedFunctionType final : public FunctionType {
public:
  constexpr UninitializedFunctionType() : FunctionType((I18n::Message)0) {}
  bool isInitialized() const override { return false; }
};

class ErrorFunctionType final : public FunctionType {
public:
  constexpr ErrorFunctionType(I18n::Message caption, Status status, SymbolType symbol) :
    FunctionType(caption),
    m_status(status),
    m_symbolType(symbol)
  {}

  Status status() const override { return m_status; }
  SymbolType symbolType() const override { return m_symbolType; }
private:
  const Status m_status;
  const SymbolType m_symbolType;
};

class CartesianEquationType : public FunctionType {
public:
  constexpr CartesianEquationType(I18n::Message caption, bool hasTwoSubCurves = false, bool isAlongY = false) :
    FunctionType(caption),
    m_hasTwoSubCurves(hasTwoSubCurves),
    m_isAlongY(isAlongY)
  {}

  SymbolType symbolType() const override { return SymbolType::X; }
  int numberOfSubCurves() const override { return 1 + m_hasTwoSubCurves; }
  bool isAlongY() const override { return m_isAlongY; }
  CurveParameterType getCurveParameterType() const override { return !m_isAlongY && !m_hasTwoSubCurves ? CurveParameterType::CartesianFunction : CurveParameterType::Default; }
private:
  const bool m_hasTwoSubCurves;
  const bool m_isAlongY;
};

class CartesianFunctionType final : public CartesianEquationType {
public:
  constexpr CartesianFunctionType(I18n::Message caption) : CartesianEquationType(caption, false, false) {}
};

class ConicEquationType final : public CartesianEquationType {
public:
  constexpr ConicEquationType(I18n::Message caption, Poincare::Conic::Shape conicType, bool hasTwoSubCurves) :
    CartesianEquationType(caption, hasTwoSubCurves, false),
    m_conicShape(conicType)
  {}
  Poincare::Conic::Shape conicShape() const override { return m_conicShape; }
private:
  Poincare::Conic::Shape m_conicShape;
};

class LineEquationType final : public CartesianEquationType {
public:
  constexpr LineEquationType(I18n::Message caption, bool isAlongY, CurveParameterType curveParameterType) :
    CartesianEquationType(caption, false, isAlongY),
    m_curveParameterType(curveParameterType)
  {}
  bool isLine() const override { return true; }
  CurveParameterType getCurveParameterType() const override { return m_curveParameterType; }
private:
  const CurveParameterType m_curveParameterType;
};

class PolarFunctionType : public FunctionType {
public:
  constexpr PolarFunctionType(I18n::Message caption) : FunctionType(caption) {}
  SymbolType symbolType() const override { return SymbolType::Theta; }
  CurveParameterType getCurveParameterType() const override { return CurveParameterType::Polar; }
};

class PolarConicFunctionType final : public PolarFunctionType {
public:
  constexpr PolarConicFunctionType(I18n::Message caption, Poincare::Conic::Shape conicShape) :
    PolarFunctionType(caption),
    m_conicShape(conicShape)
  {}
  Poincare::Conic::Shape conicShape() const override { return m_conicShape; }
private:
  Poincare::Conic::Shape m_conicShape;
};

class ParametricFunctionType : public FunctionType {
public:
  constexpr ParametricFunctionType(I18n::Message caption) : FunctionType(caption) {}
  SymbolType symbolType() const override { return SymbolType::T; }
  CurveParameterType getCurveParameterType() const override { return CurveParameterType::Parametric; }
};

class ParametricConicFunctionType final : public ParametricFunctionType {
public:
  constexpr ParametricConicFunctionType(I18n::Message caption, Poincare::Conic::Shape conicShape) :
    ParametricFunctionType(caption),
    m_conicShape(conicShape)
  {}
  Poincare::Conic::Shape conicShape() const override { return m_conicShape; }
private:
  Poincare::Conic::Shape m_conicShape;
};

// ====== ALL FUNCTION TYPES ======

class FunctionTypes {
  friend class ContinuousFunctionProperties;
private:
  // Uninitialized plot type
  constexpr static UninitializedFunctionType k_uninitializedFunctionType = UninitializedFunctionType();

  // Error plot types
  constexpr static ErrorFunctionType k_bannedFunctionType = ErrorFunctionType(I18n::Message::Disabled, FunctionType::Status::Banned, FunctionType::SymbolType::Unknown);

  constexpr static ErrorFunctionType k_unhandledCartesianFunctionType = ErrorFunctionType(I18n::Message::UnhandledType, FunctionType::Status::Unhandled, FunctionType::SymbolType::X);
  constexpr static ErrorFunctionType k_unhandledParametricFunctionType = ErrorFunctionType(I18n::Message::UnhandledType, FunctionType::Status::Unhandled, FunctionType::SymbolType::T);
  constexpr static ErrorFunctionType k_unhandledPolarFunctionType = ErrorFunctionType(I18n::Message::UnhandledType, FunctionType::Status::Unhandled, FunctionType::SymbolType::Theta);

  constexpr static ErrorFunctionType k_undefinedCartesianFunctionType = ErrorFunctionType(I18n::Message::UndefinedType, FunctionType::Status::Undefined, FunctionType::SymbolType::X);
  constexpr static ErrorFunctionType k_undefinedParametricFunctionType = ErrorFunctionType(I18n::Message::UndefinedType, FunctionType::Status::Undefined, FunctionType::SymbolType::T);
  constexpr static ErrorFunctionType k_undefinedPolarFunctionType = ErrorFunctionType(I18n::Message::UndefinedType, FunctionType::Status::Undefined, FunctionType::SymbolType::Theta);

  // Parametric
  constexpr static ParametricFunctionType k_parametricFunctionType = ParametricFunctionType(I18n::Message::ParametricEquationType);
  constexpr static ParametricFunctionType k_parametricLineType = ParametricFunctionType(I18n::Message::ParametricLineType);
  constexpr static ParametricFunctionType k_parametricHorizontalLineType = ParametricFunctionType(I18n::Message::ParametricHorizontalLineType);
  constexpr static ParametricFunctionType k_parametricVerticalLineType = ParametricFunctionType(I18n::Message::ParametricVerticalLineType);
  // Parametric conics
  constexpr static ParametricConicFunctionType k_parametricEllipseFunctionType = ParametricConicFunctionType(I18n::Message::ParametricEllipseType, Poincare::Conic::Shape::Ellipse);
  constexpr static ParametricConicFunctionType k_parametricCircleFunctionType = ParametricConicFunctionType(I18n::Message::ParametricCircleType, Poincare::Conic::Shape::Circle);
  constexpr static ParametricConicFunctionType k_parametricHyperbolaFunctionType = ParametricConicFunctionType(I18n::Message::ParametricHyperbolaType, Poincare::Conic::Shape::Hyperbola);
  constexpr static ParametricConicFunctionType k_parametricParabolaFunctionType = ParametricConicFunctionType(I18n::Message::ParametricParabolaType, Poincare::Conic::Shape::Parabola);

  // Polar
  constexpr static PolarFunctionType k_polarFunctionType = PolarFunctionType(I18n::Message::PolarEquationType);
  constexpr static PolarFunctionType k_polarLineType = PolarFunctionType(I18n::Message::PolarLineType);
  constexpr static PolarFunctionType k_polarHorizontalLineType = PolarFunctionType(I18n::Message::PolarHorizontalLineType);
  constexpr static PolarFunctionType k_polarVerticalLineType = PolarFunctionType(I18n::Message::PolarVerticalLineType);
  // Polar conics
  constexpr static PolarConicFunctionType k_polarEllipseFunctionType = PolarConicFunctionType(I18n::Message::PolarEllipseType, Poincare::Conic::Shape::Ellipse);
  constexpr static PolarConicFunctionType k_polarCircleFunctionType = PolarConicFunctionType(I18n::Message::PolarCircleType, Poincare::Conic::Shape::Circle);
  constexpr static PolarConicFunctionType k_polarHyperbolaFunctionType = PolarConicFunctionType(I18n::Message::PolarHyperbolaType, Poincare::Conic::Shape::Hyperbola);
  constexpr static PolarConicFunctionType k_polarParabolaFunctionType = PolarConicFunctionType(I18n::Message::PolarParabolaType, Poincare::Conic::Shape::Parabola);

  // Cartesian function
  // TODO: Update captions.
  constexpr static CartesianFunctionType k_cartesianFunctionType = CartesianFunctionType(I18n::Message::FunctionType);
  constexpr static CartesianFunctionType k_piecewiseFunctionType = CartesianFunctionType(I18n::Message::PiecewiseType);
  constexpr static CartesianFunctionType k_linearFunctionType = CartesianFunctionType(I18n::Message::LinearType);
  constexpr static CartesianFunctionType k_affineFunctionType = CartesianFunctionType(I18n::Message::AffineType);
  constexpr static CartesianFunctionType k_constantFunctionType = CartesianFunctionType(I18n::Message::ConstantType);
  constexpr static CartesianFunctionType k_polynomialFunctionType = CartesianFunctionType(I18n::Message::PolynomialType);
  constexpr static CartesianFunctionType k_trigonometricFunctionType = CartesianFunctionType(I18n::Message::TrigonometricType);
  constexpr static CartesianFunctionType k_exponentialFunctionType = CartesianFunctionType(I18n::Message::ExponentialType);
  constexpr static CartesianFunctionType k_logarithmicFunctionType = CartesianFunctionType(I18n::Message::LogarithmicType);
  constexpr static CartesianFunctionType k_rationalFunctionType = CartesianFunctionType(I18n::Message::RationalType);

  // Cartesian equation types
  // y = f(x)
  constexpr static CartesianFunctionType k_simpleCartesianEquationType = CartesianFunctionType(I18n::Message::EquationType);
  // y = ax + b
  constexpr static LineEquationType k_lineEquation = LineEquationType(I18n::Message::LineType, false, FunctionType::CurveParameterType::Line);
  // y = a
  constexpr static LineEquationType k_horizontalLineEquation = LineEquationType(I18n::Message::HorizontalLineType, false,  FunctionType::CurveParameterType::HorizontalLine);
  // y^2 = f(x)
  constexpr static CartesianEquationType k_cartesianEquationWithTwoSubCurves = CartesianEquationType(I18n::Message::EquationType, true);
  // Conics
  constexpr static ConicEquationType k_circleEquation = ConicEquationType(I18n::Message::CircleType, Poincare::Conic::Shape::Circle, true);
  constexpr static ConicEquationType k_ellipseEquation = ConicEquationType(I18n::Message::EllipseType, Poincare::Conic::Shape::Ellipse, true);
  constexpr static ConicEquationType k_parabolaEquationWithTwoSubCurves = ConicEquationType(I18n::Message::ParabolaType, Poincare::Conic::Shape::Parabola, true);
  constexpr static ConicEquationType k_parabolaEquationWithOneSubCurve = ConicEquationType(I18n::Message::ParabolaType, Poincare::Conic::Shape::Parabola, false);
  constexpr static ConicEquationType k_hyperbolaEquationWithTwoSubCurves = ConicEquationType(I18n::Message::HyperbolaType, Poincare::Conic::Shape::Hyperbola, true);
  constexpr static ConicEquationType k_hyperbolaEquationWithOneSubCurve = ConicEquationType(I18n::Message::HyperbolaType, Poincare::Conic::Shape::Hyperbola, false);
  // x = f(y)
  constexpr static CartesianEquationType k_cartesianEquationAlongY = CartesianEquationType(I18n::Message::EquationType, false, true);
  // x = a
  constexpr static LineEquationType k_verticalLine = LineEquationType(I18n::Message::VerticalLineType, true, FunctionType::CurveParameterType::VerticalLine);
  // x^2 = f(y)
  constexpr static CartesianEquationType k_cartesianEquationAlongYWithTwoSubCurves = CartesianEquationType(I18n::Message::EquationType, true, true);
};

}

#endif