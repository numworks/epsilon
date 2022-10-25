#ifndef SHARED_CONTINUOUS_FUNCTION_TYPES_H
#define SHARED_CONTINUOUS_FUNCTION_TYPES_H

#include <apps/i18n.h>

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

  constexpr FunctionType(I18n::Message message) : m_message(message) {}

  virtual bool isInitialized() const { return true; }
  I18n::Message message() const { assert(isInitialized()); return m_message; }
  virtual Status status() const { assert(isInitialized()); return Status::Enabled; }
  virtual SymbolType symbolType() const { assert(isInitialized()); return SymbolType::Unknown; }
  virtual int numberOfSubCurves() const { assert(isInitialized()); return 1; }
  virtual bool isAlongY() const { assert(isInitialized()); return false; }
  virtual bool isConic() const { assert(isInitialized()); return false; }
  virtual bool isLine() const { assert(isInitialized()); return false; }
  virtual CurveParameterType getCurveParameterType() const { assert(isInitialized()); return CurveParameterType::Default; }

private:
  const I18n::Message m_message;
};

class UninitializedFunctionType final : public FunctionType {
public:
  constexpr UninitializedFunctionType() : FunctionType((I18n::Message)0) {}
  bool isInitialized() const override { return false; }
};

class ErrorFunctionType final : public FunctionType {
public:
  constexpr ErrorFunctionType(I18n::Message message, Status status, SymbolType symbol) :
    FunctionType(message),
    m_status(status),
    m_symbolType(symbol)
  {}

  Status status() const override { return m_status; }
  SymbolType symbolType() const override { return m_symbolType; }
private:
  const Status m_status;
  const SymbolType m_symbolType;
};

class ParametricFunctionType final : public FunctionType {
public:
  constexpr ParametricFunctionType() : FunctionType(I18n::Message::ParametricType) {}
  SymbolType symbolType() const override { return SymbolType::T; }
  CurveParameterType getCurveParameterType() const override { return CurveParameterType::Parametric; }
};

class PolarFunctionType final : public FunctionType {
public:
  constexpr PolarFunctionType() : FunctionType(I18n::Message::PolarType) {}
  SymbolType symbolType() const override { return SymbolType::Theta; }
  CurveParameterType getCurveParameterType() const override { return CurveParameterType::Polar; }
};

class CartesianEquationType : public FunctionType {
public:
  constexpr CartesianEquationType(I18n::Message message, bool hasTwoSubCurves = false, bool isAlongY = false) :
    FunctionType(message),
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
  constexpr CartesianFunctionType(I18n::Message message) : CartesianEquationType(message, false, false) {}
};

class ConicEquationType final : public CartesianEquationType {
public:
  constexpr ConicEquationType(I18n::Message message, bool hasTwoSubCurves) :
    CartesianEquationType(message, hasTwoSubCurves, false)
  {}
  bool isConic() const override { return true; }
};

class LineEquationType final : public CartesianEquationType {
public:
  constexpr LineEquationType(I18n::Message message, bool isAlongY, CurveParameterType curveParameterType) :
    CartesianEquationType(message, false, isAlongY),
    m_curveParameterType(curveParameterType)
  {}
  bool isLine() const override { return true; }
  CurveParameterType getCurveParameterType() const override { return m_curveParameterType; }
private:
  const CurveParameterType m_curveParameterType;
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
  constexpr static ParametricFunctionType k_parametricFunctionType = ParametricFunctionType();

  // Polar
  constexpr static PolarFunctionType k_polarFunctionType = PolarFunctionType();

  // Cartesian function
  // TODO: Update messages.
  constexpr static CartesianFunctionType k_cartesianFunctionType = CartesianFunctionType(I18n::Message::CartesianType);
  constexpr static CartesianFunctionType k_piecewiseFunctionType = CartesianFunctionType(I18n::Message::ColorBlue);
  constexpr static CartesianFunctionType k_linearFunctionType = CartesianFunctionType(I18n::Message::ColorGreen);
  constexpr static CartesianFunctionType k_affineFunctionType = CartesianFunctionType(I18n::Message::ColorRed);
  constexpr static CartesianFunctionType k_constantFunctionType = CartesianFunctionType(I18n::Message::ColorYellowDark);
  constexpr static CartesianFunctionType k_polynomialFunctionType = CartesianFunctionType(I18n::Message::ColorOrange);
  constexpr static CartesianFunctionType k_trigonometricFunctionType = CartesianFunctionType(I18n::Message::ColorMagenta);
  constexpr static CartesianFunctionType k_exponentialFunctionType = CartesianFunctionType(I18n::Message::ColorPink);
  constexpr static CartesianFunctionType k_logarithmicFunctionType = CartesianFunctionType(I18n::Message::Ceiling);
  constexpr static CartesianFunctionType k_rationalFunctionType = CartesianFunctionType(I18n::Message::Rounding);

  // Cartesian equation types
  // y = f(x)
  constexpr static CartesianFunctionType k_simpleCartesianEquationType = CartesianFunctionType(/* TODO: Change message */I18n::Message::CartesianType);
  // y = ax + b
  constexpr static LineEquationType k_lineEquation = LineEquationType(I18n::Message::LineType, false, FunctionType::CurveParameterType::Line);
  // y = a
  constexpr static LineEquationType k_horizontalLineEquation = LineEquationType(I18n::Message::HorizontalLineType, false,  FunctionType::CurveParameterType::HorizontalLine);
  // y^2 = f(x)
  constexpr static CartesianEquationType k_cartesianEquationWithTwoSubCurves = CartesianEquationType(I18n::Message::OtherType, true);
  // Conics
  constexpr static ConicEquationType k_circleEquation = ConicEquationType(I18n::Message::CircleType, true);
  constexpr static ConicEquationType k_ellipseEquation = ConicEquationType(I18n::Message::EllipseType, true);
  constexpr static ConicEquationType k_parabolaEquationWithTwoSubCurves = ConicEquationType(I18n::Message::ParabolaType, true);
  constexpr static ConicEquationType k_parabolaEquationWithOneSubCurve = ConicEquationType(I18n::Message::ParabolaType, false);
  constexpr static ConicEquationType k_hyperbolaEquationWithTwoSubCurves = ConicEquationType(I18n::Message::HyperbolaType, true);
  constexpr static ConicEquationType k_hyperbolaEquationWithOneSubCurve = ConicEquationType(I18n::Message::HyperbolaType, false);
  // x = f(y)
  constexpr static CartesianEquationType k_cartesianEquationAlongY = CartesianEquationType(I18n::Message::InverseType, false, true);
  // x = a
  constexpr static LineEquationType k_verticalLine = LineEquationType(I18n::Message::VerticalLineType, true, FunctionType::CurveParameterType::VerticalLine);
  // x^2 = f(y)
  constexpr static CartesianEquationType k_cartesianEquationAlongYWithTwoSubCurves = CartesianEquationType(I18n::Message::InverseType, true, true);
};

}

#endif