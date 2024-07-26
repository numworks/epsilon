#include "continuous_function_properties.h"

#include <omg/unreachable.h>
#include <poincare/cas.h>
#include <poincare/code_points.h>
#include <poincare/function_properties/function_type.h>
#include <poincare/helpers/symbol.h>
#include <poincare/src/expression/polynomial.h>

#include "continuous_function.h"

using namespace Poincare;
using namespace Poincare::CodePoints;

namespace Shared {

bool ContinuousFunctionProperties::parameterAtIndexIsEditable(int index) const {
  // TODO(lorene): use a ParameterIndex input type to avoid hardcoded int values
  assert(isEnabled());
  assert(index < numberOfCurveParameters());
  CurveParameterType curveParameterType = getCurveParameterType();
  switch (curveParameterType) {
    case CurveParameterType::CartesianFunction:
    case CurveParameterType::Line:
      return true;
    case CurveParameterType::VerticalLine:
      return index == 1;
    case CurveParameterType::HorizontalLine:
    case CurveParameterType::Parametric:
    case CurveParameterType::Polar:
    case CurveParameterType::InversePolar:
      return index == 0;
    default:
      return false;
  }
}

bool ContinuousFunctionProperties::parameterAtIndexIsPreimage(int index) const {
  // TODO(lorene): use a ParameterIndex input type to avoid hardcoded int values
  assert(isEnabled());
  assert(index < numberOfCurveParameters());
  CurveParameterType curveParameterType = getCurveParameterType();
  return (curveParameterType == CurveParameterType::CartesianFunction ||
          curveParameterType == CurveParameterType::Line) &&
         index == 1;
}

ContinuousFunctionProperties::AreaType ContinuousFunctionProperties::areaType()
    const {
  assert(isInitialized());
  if (!isEnabled() || equationType() == ComparisonJunior::Operator::Equal) {
    return AreaType::None;
  }
  // To draw y^2>a, the area plotted should be Outside and not Above.
  if (equationType() == ComparisonJunior::Operator::Inferior ||
      equationType() == ComparisonJunior::Operator::InferiorEqual) {
    return isOfDegreeTwo() ? AreaType::Inside : AreaType::Below;
  }
  assert(equationType() == ComparisonJunior::Operator::Superior ||
         equationType() == ComparisonJunior::Operator::SuperiorEqual);
  return isOfDegreeTwo() ? AreaType::Outside : AreaType::Above;
}

CodePoint ContinuousFunctionProperties::symbol() const {
  switch (symbolType()) {
    case SymbolType::T:
      return k_parametricSymbol;
    case SymbolType::Theta:
      return k_polarSymbol;
    case SymbolType::Radius:
      return k_radiusSymbol;
    case SymbolType::NoSymbol:
      return k_cartesianSymbol;
    default:
      assert(symbolType() == SymbolType::X);
      return k_cartesianSymbol;
  }
}

ContinuousFunctionProperties::SymbolType
ContinuousFunctionProperties::SymbolTypeForCodePoint(CodePoint symbol) {
  switch (symbol) {
    case k_parametricSymbol:
      return SymbolType::T;
    case k_polarSymbol:
      return SymbolType::Theta;
    default:
      assert(symbol == k_cartesianSymbol);
      return SymbolType::X;
  }
}

I18n::Message ContinuousFunctionProperties::MessageForSymbolType(
    SymbolType symbolType) {
  switch (symbolType) {
    case SymbolType::T:
      return I18n::Message::T;
    case SymbolType::Theta:
      return I18n::Message::Theta;
    case SymbolType::Radius:
      return I18n::Message::R;
    case SymbolType::NoSymbol:
      return I18n::Message::Default;
    default:
      assert(symbolType == SymbolType::X);
      return I18n::Message::X;
  }
}

void ContinuousFunctionProperties::reset() {
  m_isInitialized = false;
  setCaption(k_defaultCaption);
  setStatus(k_defaultStatus);
  setEquationType(k_defaultEquationType);
  setSymbolType(k_defaultSymbolType);
  setCurveParameterType(k_defaultCurveParameterType);
  setConicShape(k_defaultConicShape);
  setIsOfDegreeTwo(k_defaultIsOfDegreeTwo);
  setIsAlongY(k_defaultIsAlongY);
}

void ContinuousFunctionProperties::setErrorStatusAndUpdateCaption(
    Status status) {
  assert(status != Status::Enabled);
  setStatus(status);
  switch (status) {
    case Status::Banned:
      setCaption(I18n::Message::Disabled);
      return;
    case Status::Undefined:
      setCaption(I18n::Message::UndefinedType);
      return;
    default:
      assert(status == Status::Unhandled);
      setCaption(I18n::Message::UnhandledType);
      return;
  }
}

void ContinuousFunctionProperties::update(
    const Poincare::SystemExpression reducedEquation,
    const Poincare::UserExpression inputEquation, Context* context,
    Preferences::ComplexFormat complexFormat,
    ComparisonJunior::Operator precomputedOperatorType,
    SymbolType precomputedFunctionSymbol, bool isCartesianEquation) {
  reset();
  m_isInitialized = true;

  setSymbolType(precomputedFunctionSymbol);
  setEquationType(precomputedOperatorType);

  if (Preferences::SharedPreferences()->examMode().forbidInequalityGraphing() &&
      precomputedOperatorType != ComparisonJunior::Operator::Equal) {
    setErrorStatusAndUpdateCaption(Status::Banned);
    return;
  }

  /* We do not care about reduced expression since it is never shown to the
   * user. We do not care (neither have) an approximate expression. Indeed we
   * only check display permissions for input expression.*/
  bool genericCaptionOnly = CAS::ShouldOnlyDisplayApproximation(
      inputEquation, UserExpression(), UserExpression(), context);

  setHideDetails(genericCaptionOnly);

  assert(!reducedEquation.isUninitialized());
  if (reducedEquation.isUndefined()) {
    setErrorStatusAndUpdateCaption(Status::Undefined);
    return;
  }

  // Do not handle dependencies for now.
  const SystemExpression analyzedExpression =
      (reducedEquation.isDep()) ? reducedEquation.cloneChildAtIndex(0)
                                : reducedEquation;
  if (reducedEquation.isDep() && analyzedExpression.isDep()) {
    // If there is still a dependency, it means that the reduction failed.
    setErrorStatusAndUpdateCaption(Status::Unhandled);
    return;
  }

  // Compute equation's degree regarding y.
  int yDeg = analyzedExpression.polynomialDegree(k_ordinateName);
  if (!isCartesianEquation) {
    // There should be no y symbol. Inequations are handled on cartesians only
    if (yDeg > 0 ||
        (precomputedOperatorType != ComparisonJunior::Operator::Equal &&
         precomputedFunctionSymbol != SymbolType::X)) {
      setErrorStatusAndUpdateCaption(Status::Unhandled);
      return;
    }

    // Check dimension
    Dimension dimension = analyzedExpression.dimension(context);
    if (((precomputedFunctionSymbol == SymbolType::X ||
          precomputedFunctionSymbol == SymbolType::Theta ||
          precomputedFunctionSymbol == SymbolType::Radius) &&
         !dimension.isScalar()) ||
        (precomputedFunctionSymbol == SymbolType::T && !dimension.isPoint()) ||
        (precomputedFunctionSymbol == SymbolType::NoSymbol &&
         !dimension.isPointOrListOfPoints())) {
      setErrorStatusAndUpdateCaption(Status::Undefined);
      return;
    }

    switch (precomputedFunctionSymbol) {
      case SymbolType::X: {
        assert(dimension.isScalar());
        setCurveParameterType(CurveParameterType::CartesianFunction);
        setCaption(captionForCartesianFunction(analyzedExpression));
        if (genericCaptionOnly) {
          setCaption(I18n::Message::Function);
        }
        return;
      }
      case SymbolType::Theta: {
        assert(dimension.isScalar());
        setCurveParameterType(CurveParameterType::Polar);
        setCaption(captionForPolarFunction(analyzedExpression));
        if (genericCaptionOnly) {
          setCaption(I18n::Message::PolarEquationType);
        }
        return;
      }
      case SymbolType::Radius: {
        assert(dimension.isScalar());
        // TODO: Inverse polar could also be analyzed
        setCurveParameterType(CurveParameterType::InversePolar);
        setCaption(I18n::Message::PolarEquationType);
        return;
      }
      case SymbolType::T: {
        assert(dimension.isPoint());
        setCurveParameterType(CurveParameterType::Parametric);
        setCaption(captionForParametricFunction(analyzedExpression));
        if (genericCaptionOnly) {
          setCaption(I18n::Message::ParametricEquationType);
        }
        return;
      }
      case SymbolType::NoSymbol: {
        assert(dimension.isPointOrListOfPoints());
        setCaption(dimension.isPoint() ? I18n::Message::PointType
                                       : I18n::Message::ListOfPointsType);
        setCurveParameterType(CurveParameterType::ScatterPlot);
        return;
      }
      default:
        OMG::unreachable();
    }
  }

  // Compute equation's degree regarding x.
  int xDeg = analyzedExpression.polynomialDegree(Function::k_unknownName);

  bool willBeAlongX = (yDeg == 1) || (yDeg == 2);
  bool willBeAlongY = !willBeAlongX && ((xDeg == 1) || (xDeg == 2));
  if (!willBeAlongX && !willBeAlongY) {
    // Any equation with such a y and x degree won't be handled anyway.
    setErrorStatusAndUpdateCaption(Status::Unhandled);
    return;
  }

  const char* symbolName =
      willBeAlongX ? k_ordinateName : Function::k_unknownName;
  OMG::Troolean highestCoefficientIsPositive = OMG::Troolean::Unknown;
  if (!Poincare::Internal::PolynomialParser::HasNonNullCoefficients(
          analyzedExpression.tree(), symbolName,
          &highestCoefficientIsPositive)) {
    // The equation must have at least one nonNull coefficient.
    setErrorStatusAndUpdateCaption(Status::Unhandled);
    return;
  }

  if (precomputedOperatorType != ComparisonJunior::Operator::Equal) {
    if (highestCoefficientIsPositive == OMG::Troolean::Unknown ||
        (yDeg == 2 && xDeg == -1)) {
      /* Are unhandled equation with :
       * - An unknown highest coefficient sign: sign must be strict and constant
       * - A non polynomial x coefficient in a quadratic equation on y. */
      setErrorStatusAndUpdateCaption(Status::Unhandled);
      return;
    }
    if (highestCoefficientIsPositive == OMG::Troolean::False) {
      // Oppose the comparison operator
      precomputedOperatorType =
          ComparisonJunior::OperatorReverseInferiorSuperior(
              precomputedOperatorType);
      setEquationType(precomputedOperatorType);
    }
  }

  if (Preferences::SharedPreferences()->examMode().forbidImplicitPlots()) {
    CodePoint symbol = willBeAlongX ? k_ordinateSymbol : UCodePointUnknown;
    if (!IsExplicitEquation(inputEquation, symbol)) {
      setErrorStatusAndUpdateCaption(Status::Banned);
      return;
    }
  }

  assert(analyzedExpression.dimension(context).isScalar());
  setCartesianEquationProperties(analyzedExpression, xDeg, yDeg,
                                 highestCoefficientIsPositive);
  if (genericCaptionOnly) {
    setCaption(I18n::Message::Equation);
  }
}

I18n::Message ContinuousFunctionProperties::captionForCartesianFunction(
    const SystemExpression& analyzedExpression) {
  assert(!analyzedExpression.isDep());
  assert(isEnabled() && isCartesian());
  FunctionType::CartesianType type = FunctionType::CartesianFunctionType(
      analyzedExpression, Function::k_unknownName);
  switch (type) {
    case FunctionType::CartesianType::Piecewise:
      return I18n::Message::PiecewiseType;
    case FunctionType::CartesianType::Constant:
      return I18n::Message::ConstantType;
    case FunctionType::CartesianType::Affine:
      return I18n::Message::AffineType;
    case FunctionType::CartesianType::Linear:
      return I18n::Message::LinearType;
    case FunctionType::CartesianType::Polynomial:
      return I18n::Message::PolynomialType;
    case FunctionType::CartesianType::Logarithmic:
      return I18n::Message::LogarithmicType;
    case FunctionType::CartesianType::Exponential:
      return I18n::Message::ExponentialType;
    case FunctionType::CartesianType::Rational:
      return I18n::Message::RationalType;
    case FunctionType::CartesianType::Trigonometric:
      return I18n::Message::TrigonometricType;
    case FunctionType::CartesianType::Default:
      return I18n::Message::Function;
    default:
      OMG::unreachable();
  }
}

void ContinuousFunctionProperties::setCartesianEquationProperties(
    const Poincare::SystemExpression& analyzedExpression, int xDeg, int yDeg,
    OMG::Troolean highestCoefficientIsPositive) {
  assert(!analyzedExpression.isDep());
  assert(isEnabled() && isCartesian());

  /* We can rely on x and y degree to identify plot type :
   * | y  | x  | Status
   * | 0  | 1  | Vertical Line
   * | 0  | 2  | Vertical Lines
   * | 1  | 0  | Horizontal Line
   * | 1  | 1  | Line
   * | 1  | *  | Cartesian
   * | 2  | 0  | Other (Two Horizontal Lines)
   * | 2  | 1  | Circle, Ellipsis, Hyperbola, Parabola, Other
   * | 2  | 2  | Circle, Ellipsis, Hyperbola, Parabola, Other
   * | 2  | *  | Other
   * | *  | 1  | CartesianAlongY
   *
   * Other cases should have been escaped before.
   */

  setCaption(I18n::Message::Equation);

  if (yDeg != 1 && yDeg != 2) {  // function is along Y
    setIsAlongY(true);
    setCaption(I18n::Message::Equation);
    if (xDeg == 2) {
      setIsOfDegreeTwo(true);
    } else if (yDeg == 0) {
      setCaption(I18n::Message::VerticalLineType);
      setCurveParameterType(CurveParameterType::VerticalLine);
    } else {
      assert(xDeg == 1);
    }
    return;
  }

  assert(yDeg == 2 || yDeg == 1);

  if (yDeg == 1 && xDeg == 0) {
    setCaption(I18n::Message::HorizontalLineType);
    setCurveParameterType(CurveParameterType::HorizontalLine);
    return;
  }

  if (yDeg == 1 && xDeg == 1 &&
      highestCoefficientIsPositive != OMG::Troolean::Unknown) {
    // An Unknown y coefficient sign might mean it depends on x (y*x = ...)
    setCaption(I18n::Message::LineType);
    setCurveParameterType(CurveParameterType::Line);
    return;
  }

  setIsOfDegreeTwo(yDeg == 2);
  setCurveParameterType(yDeg == 2 ? CurveParameterType::Default
                                  : CurveParameterType::CartesianFunction);

  if (xDeg >= 1 && xDeg <= 2 &&
      !Preferences::SharedPreferences()->examMode().forbidImplicitPlots()) {
    /* If implicit plots are forbidden, ignore conics (such as y=x^2) to hide
     * details. Otherwise, try to identify a conic.
     * For instance, x*y=1 as an hyperbola. */
    CartesianConic equationConic =
        CartesianConic(analyzedExpression, Function::k_unknownName);
    setConicShape(equationConic.conicType().shape);
    switch (conicShape()) {
      case Conic::Shape::Hyperbola:
        setCaption(I18n::Message::HyperbolaType);
        return;
      case Conic::Shape::Parabola:
        setCaption(I18n::Message::ParabolaType);
        return;
      case Conic::Shape::Ellipse:
        setCaption(I18n::Message::EllipseType);
        return;
      case Conic::Shape::Circle:
        setCaption(I18n::Message::CircleType);
        return;
      default:;
        // A conic could not be identified.
    }
  }
}

I18n::Message ContinuousFunctionProperties::captionForPolarFunction(
    const SystemExpression& analyzedExpression) {
  assert(!analyzedExpression.isDep());
  assert(isEnabled() && isPolar());

  // Detect polar lines
  FunctionType::LineType lineType =
      FunctionType::PolarLineType(analyzedExpression, Function::k_unknownName);
  switch (lineType) {
    case FunctionType::LineType::Vertical:
      return I18n::Message::PolarVerticalLineType;
    case FunctionType::LineType::Horizontal:
      return I18n::Message::PolarHorizontalLineType;
    case FunctionType::LineType::Diagonal:
      return I18n::Message::PolarLineType;
    default:
      assert(lineType == FunctionType::LineType::None);
  }

  // Detect polar conics
  PolarConic conicProperties =
      PolarConic(analyzedExpression, Function::k_unknownName);
  setConicShape(conicProperties.conicType().shape);
  switch (conicShape()) {
    case Conic::Shape::Hyperbola:
      return I18n::Message::PolarHyperbolaType;
    case Conic::Shape::Parabola:
      return I18n::Message::PolarParabolaType;
    case Conic::Shape::Ellipse:
      return I18n::Message::PolarEllipseType;
    case Conic::Shape::Circle:
      return I18n::Message::PolarCircleType;
    default:
      // A conic could not be identified.
      return I18n::Message::PolarEquationType;
  }
}

I18n::Message ContinuousFunctionProperties::captionForParametricFunction(
    const Poincare::SystemExpression& analyzedExpression) {
  assert(analyzedExpression.isPoint());
  assert(isEnabled() && isParametric());

  // Detect parametric lines
  FunctionType::LineType lineType = FunctionType::ParametricLineType(
      analyzedExpression, Function::k_unknownName);
  switch (lineType) {
    case FunctionType::LineType::Vertical:
      /* The same text as cartesian equation is used because the caption
       * "Parametric equation of a vertical line" is too long to fit
       * the 37 max chars limit in every language.
       * This can be changed later if more chars are available. */
      return I18n::Message::VerticalLineType;
    case FunctionType::LineType::Horizontal:
      /* Same comment as above. */
      return I18n::Message::HorizontalLineType;
    case FunctionType::LineType::Diagonal:
      return I18n::Message::ParametricLineType;
    default:
      assert(lineType == FunctionType::LineType::None);
  }

  // Detect parametric conics
  ParametricConic conicProperties =
      ParametricConic(analyzedExpression, Function::k_unknownName);
  setConicShape(conicProperties.conicType().shape);
  switch (conicShape()) {
    case Conic::Shape::Parabola:
      return I18n::Message::ParametricParabolaType;
    case Conic::Shape::Ellipse:
      return I18n::Message::ParametricEllipseType;
    case Conic::Shape::Circle:
      return I18n::Message::ParametricCircleType;
    default:
      // For now, these are not detected and there is no caption for it.
      assert(conicShape() != Conic::Shape::Hyperbola);
      // A conic could not be identified.
      return I18n::Message::ParametricEquationType;
  }
}

bool ContinuousFunctionProperties::IsExplicitEquation(
    const SystemExpression equation, CodePoint symbol) {
  /* An equation is explicit if it is a comparison between the given symbol and
   * something that does not depend on it. For example, using 'y' symbol:
   * y=1+x or y>x are explicit but y+1=x or y=x+2*y are implicit. */
  return equation.isComparison() &&
         SymbolHelper::IsSymbol(equation.cloneChildAtIndex(0), symbol) &&
         !equation.cloneChildAtIndex(1).recursivelyMatches(
             [](const NewExpression e, Context* context, void* auxiliary) {
               const CodePoint* symbol =
                   static_cast<const CodePoint*>(auxiliary);
               return (!e.isUninitialized() &&
                       SymbolHelper::IsSymbol(e, *symbol))
                          ? OMG::Troolean::True
                          : OMG::Troolean::Unknown;
             },
             nullptr, SymbolicComputation::KeepAllSymbols,
             static_cast<void*>(&symbol));
}

}  // namespace Shared
