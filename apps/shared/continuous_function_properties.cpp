#include "continuous_function_properties.h"

#include <apps/shared/expression_display_permissions.h>
#include <omg/unreachable.h>
#include <poincare/function_properties_helper.h>
#include <poincare/src/expression/polynomial.h>

#include "continuous_function.h"

using namespace Poincare;

namespace Shared {

bool ContinuousFunctionProperties::parameterAtIndexIsEditable(int index) const {
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
  if (!isEnabled() || equationType() == ComparisonNode::OperatorType::Equal) {
    return AreaType::None;
  }
  // To draw y^2>a, the area plotted should be Outside and not Above.
  if (equationType() == ComparisonNode::OperatorType::Inferior ||
      equationType() == ComparisonNode::OperatorType::InferiorEqual) {
    return isOfDegreeTwo() ? AreaType::Inside : AreaType::Below;
  }
  assert(equationType() == ComparisonNode::OperatorType::Superior ||
         equationType() == ComparisonNode::OperatorType::SuperiorEqual);
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
    ComparisonNode::OperatorType precomputedOperatorType,
    SymbolType precomputedFunctionSymbol, bool isCartesianEquation) {
  reset();
  m_isInitialized = true;

  setSymbolType(precomputedFunctionSymbol);
  setEquationType(precomputedOperatorType);

  if (Preferences::SharedPreferences()->examMode().forbidInequalityGraphing() &&
      precomputedOperatorType != ComparisonNode::OperatorType::Equal) {
    setErrorStatusAndUpdateCaption(Status::Banned);
    return;
  }

  /* We do not care about reduced expression since it is never shown to the
   * user. We do not care (neither have) an approximate expression. Indeed we
   * only check display permissions for input expression.*/
  bool genericCaptionOnly =
      Shared::ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
          inputEquation, UserExpression(), UserExpression(), context);

  setHideDetails(genericCaptionOnly);

  assert(!reducedEquation.isUninitialized());
  if (reducedEquation.type() == ExpressionNode::Type::Undefined) {
    setErrorStatusAndUpdateCaption(Status::Undefined);
    return;
  }

  SystemExpression analyzedExpression = reducedEquation;
  if (reducedEquation.type() == ExpressionNode::Type::Dependency) {
    // Do not handle dependencies for now.
    analyzedExpression = reducedEquation.childAtIndex(0);

    // If there is still a dependency, it means that the reduction failed.
    if (analyzedExpression.type() == ExpressionNode::Type::Dependency) {
      setErrorStatusAndUpdateCaption(Status::Unhandled);
      return;
    }
  }

  Internal::ProjectionContext projectionContext = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = Internal::AngleUnit::Radian,
      .m_unitFormat = Internal::UnitFormat::Metric,
      .m_symbolic =
          Internal::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      .m_context = context,
      .m_unitDisplay = Internal::UnitDisplay::None,
  };

  // Compute equation's degree regarding y.
  int yDeg = analyzedExpression.polynomialDegree(context, k_ordinateName);
  if (!isCartesianEquation) {
    // There should be no y symbol. Inequations are handled on cartesians only
    if (yDeg > 0 ||
        (precomputedOperatorType != ComparisonNode::OperatorType::Equal &&
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
        setCartesianFunctionProperties(analyzedExpression, projectionContext);
        if (genericCaptionOnly) {
          setCaption(I18n::Message::Function);
        }
        return;
      }
      case SymbolType::Theta: {
        setPolarFunctionProperties(analyzedExpression, projectionContext);
        if (genericCaptionOnly) {
          setCaption(I18n::Message::PolarEquationType);
        }
        return;
      }
      case SymbolType::Radius: {
        assert(dimension.isScalar());
        // TODO: Inverse polar could also be analyzed
        setCaption(I18n::Message::PolarEquationType);
        setCurveParameterType(CurveParameterType::InversePolar);
        return;
      }
      case SymbolType::T: {
        setParametricFunctionProperties(analyzedExpression, projectionContext);
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
  int xDeg =
      analyzedExpression.polynomialDegree(context, Function::k_unknownName);

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
          analyzedExpression.tree(), symbolName, projectionContext,
          &highestCoefficientIsPositive)) {
    // The equation must have at least one nonNull coefficient.
    setErrorStatusAndUpdateCaption(Status::Unhandled);
    return;
  }

  if (precomputedOperatorType != ComparisonNode::OperatorType::Equal) {
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
          ComparisonNode::SwitchInferiorSuperior(precomputedOperatorType);
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

  setCartesianEquationProperties(analyzedExpression, projectionContext, xDeg,
                                 yDeg, highestCoefficientIsPositive);
  if (genericCaptionOnly) {
    setCaption(I18n::Message::Equation);
  }
}

void ContinuousFunctionProperties::setCartesianFunctionProperties(
    const SystemExpression& analyzedExpression,
    Internal::ProjectionContext projectionContext) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(isEnabled() && isCartesian());
  assert(analyzedExpression.dimension(projectionContext.m_context).isScalar());

  setCurveParameterType(CurveParameterType::CartesianFunction);

  FunctionPropertiesHelper::FunctionType type =
      FunctionPropertiesHelper::CartesianFunctionType(
          analyzedExpression, Function::k_unknownName, projectionContext);
  switch (type) {
    case FunctionPropertiesHelper::FunctionType::Piecewise:
      setCaption(I18n::Message::PiecewiseType);
      return;
    case FunctionPropertiesHelper::FunctionType::Constant:
      setCaption(I18n::Message::ConstantType);
      return;
    case FunctionPropertiesHelper::FunctionType::Affine:
      setCaption(I18n::Message::AffineType);
      return;
    case FunctionPropertiesHelper::FunctionType::Linear:
      setCaption(I18n::Message::LinearType);
      return;
    case FunctionPropertiesHelper::FunctionType::Polynomial:
      setCaption(I18n::Message::PolynomialType);
      return;
    case FunctionPropertiesHelper::FunctionType::Logarithmic:
      setCaption(I18n::Message::LogarithmicType);
      return;
    case FunctionPropertiesHelper::FunctionType::Exponential:
      setCaption(I18n::Message::ExponentialType);
      return;
    case FunctionPropertiesHelper::FunctionType::Rational:
      setCaption(I18n::Message::RationalType);
      return;
    case FunctionPropertiesHelper::FunctionType::Trigonometric:
      setCaption(I18n::Message::TrigonometricType);
      return;
    case FunctionPropertiesHelper::FunctionType::Default:
      setCaption(I18n::Message::Function);
      return;
    default:
      OMG::unreachable();
  }
}

void ContinuousFunctionProperties::setCartesianEquationProperties(
    const Poincare::SystemExpression& analyzedExpression,
    Internal::ProjectionContext projectionContext, int xDeg, int yDeg,
    OMG::Troolean highestCoefficientIsPositive) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(isEnabled() && isCartesian());
  assert(analyzedExpression.dimension(projectionContext.m_context).isScalar());

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
    CartesianConic equationConic = CartesianConic(
        analyzedExpression, projectionContext.m_context,
        projectionContext.m_complexFormat, Function::k_unknownName);
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

void ContinuousFunctionProperties::setPolarFunctionProperties(
    const SystemExpression& analyzedExpression,
    Internal::ProjectionContext projectionContext) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(isEnabled() && isPolar());
  assert(analyzedExpression.dimension(projectionContext.m_context).isScalar());

  setCurveParameterType(CurveParameterType::Polar);

  // Detect polar lines
  FunctionPropertiesHelper::LineType lineType =
      FunctionPropertiesHelper::PolarLineType(
          analyzedExpression, Function::k_unknownName, projectionContext);
  switch (lineType) {
    case FunctionPropertiesHelper::LineType::Vertical:
      setCaption(I18n::Message::PolarVerticalLineType);
      return;
    case FunctionPropertiesHelper::LineType::Horizontal:
      setCaption(I18n::Message::PolarHorizontalLineType);
      return;
    case FunctionPropertiesHelper::LineType::Diagonal:
      setCaption(I18n::Message::PolarLineType);
      return;
    default:
      assert(lineType == FunctionPropertiesHelper::LineType::None);
  }

  // Detect polar conics
  PolarConic conicProperties =
      PolarConic(analyzedExpression, projectionContext.m_context,
                 projectionContext.m_complexFormat, Function::k_unknownName);
  setConicShape(conicProperties.conicType().shape);
  switch (conicShape()) {
    case Conic::Shape::Hyperbola:
      setCaption(I18n::Message::PolarHyperbolaType);
      return;
    case Conic::Shape::Parabola:
      setCaption(I18n::Message::PolarParabolaType);
      return;
    case Conic::Shape::Ellipse:
      setCaption(I18n::Message::PolarEllipseType);
      return;
    case Conic::Shape::Circle:
      setCaption(I18n::Message::PolarCircleType);
      return;
    default:
      // A conic could not be identified.
      setCaption(I18n::Message::PolarEquationType);
  }
}

void ContinuousFunctionProperties::setParametricFunctionProperties(
    const Poincare::SystemExpression& analyzedExpression,
    Internal::ProjectionContext projectionContext) {
  assert(analyzedExpression.type() == ExpressionNode::Type::Point);
  assert(isEnabled() && isParametric());

  setCurveParameterType(CurveParameterType::Parametric);
  setCaption(I18n::Message::ParametricEquationType);

  // Detect parametric lines
  FunctionPropertiesHelper::LineType lineType =
      FunctionPropertiesHelper::ParametricLineType(
          analyzedExpression, Function::k_unknownName, projectionContext);
  switch (lineType) {
    case FunctionPropertiesHelper::LineType::Vertical:
      /* The same text as cartesian equation is used because the caption
       * "Parametric equation of a vertical line" is too long to fit
       * the 37 max chars limit in every language.
       * This can be changed later if more chars are available. */
      setCaption(I18n::Message::VerticalLineType);
      return;
    case FunctionPropertiesHelper::LineType::Horizontal:
      /* Same comment as above. */
      setCaption(I18n::Message::HorizontalLineType);
      return;
    case FunctionPropertiesHelper::LineType::Diagonal:
      setCaption(I18n::Message::ParametricLineType);
      return;
    default:
      assert(lineType == FunctionPropertiesHelper::LineType::None);
  }

  // Detect parametric conics
  ParametricConic conicProperties = ParametricConic(
      analyzedExpression, projectionContext, Function::k_unknownName);
  setConicShape(conicProperties.conicType().shape);
  switch (conicShape()) {
    case Conic::Shape::Hyperbola:
      // For now, these are not detected and there is no caption for it.
      assert(false);
      return;
    case Conic::Shape::Parabola:
      setCaption(I18n::Message::ParametricParabolaType);
      return;
    case Conic::Shape::Ellipse:
      setCaption(I18n::Message::ParametricEllipseType);
      return;
    case Conic::Shape::Circle:
      setCaption(I18n::Message::ParametricCircleType);
      return;
    default:;
      // A conic could not be identified.
  }
}

bool ContinuousFunctionProperties::IsExplicitEquation(
    const SystemExpression equation, CodePoint symbol) {
  /* An equation is explicit if it is a comparison between the given symbol and
   * something that does not depend on it. For example, using 'y' symbol:
   * y=1+x or y>x are explicit but y+1=x or y=x+2*y are implicit. */
  return equation.type() == ExpressionNode::Type::Comparison &&
         equation.childAtIndex(0).isIdenticalTo(Symbol::Builder(symbol)) &&
         !equation.childAtIndex(1).recursivelyMatches(
             [](const NewExpression e, Context* context, void* auxiliary) {
               const CodePoint* symbol =
                   static_cast<const CodePoint*>(auxiliary);
               return (!e.isUninitialized() &&
                       e.isIdenticalTo(Symbol::Builder(*symbol)))
                          ? OMG::Troolean::True
                          : OMG::Troolean::Unknown;
             },
             nullptr, SymbolicComputation::DoNotReplaceAnySymbol,
             static_cast<void*>(&symbol));
}

}  // namespace Shared
