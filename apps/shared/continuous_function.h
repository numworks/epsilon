#ifndef SHARED_CONTINUOUS_FUNCTION_H
#define SHARED_CONTINUOUS_FUNCTION_H

/* Although the considered functions are not generally continuous
 * mathematically speaking, the present class is named ContinuousFunction to
 * mark the difference with the Sequence class.
 *
 * We could not simply name it Function, since such a class already exists:
 * it is the base class of ContinuousFunction and Sequence.
 */

#include "function.h"
#include "range_1D.h"
#include <apps/i18n.h>
#include <poincare/symbol_abstract.h>
#include <poincare/conic.h>
#include <poincare/preferences.h>
#include "continuous_function_cache.h"

namespace Shared {

class ContinuousFunction : public Function {
  /* We want the cache to be able to call privateEvaluateXYAtParameter to
   * bypass cache lookup when memoizing the function's values. */
  friend class ContinuousFunctionCache;
public:
  /* ContinuousFunction */

  constexpr static int k_maxDefaultNameSize = sizeof("f99");

  // Create a record with baseName
  static ContinuousFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);
  // Builder
  ContinuousFunction(Ion::Storage::Record record = Record()) : Function(record), m_cache(nullptr) {}

  /* Area, Symbol and Plot types */

  enum class AreaType : uint8_t {
    /* Which area to fill (#) around the curve (|). For example:
     *  Equation:      x^2-1    x^2     x^2+1    x      */
    None = 0, //  =0    | |      |               |
    Above,    //  >0     -       -        -      |#
    Below,    //  <0     -       -        -     #|
    Inside,   //  <0    |#|      |               -
    Outside   //  >0   #| |#    #|#       #      -
  };

  constexpr static size_t k_numberOfSymbolTypes = 3;
  enum class SymbolType : uint8_t {
    Theta = 0,
    T,
    X
  };

  constexpr static size_t k_numberOfPlotTypes = 23;
  enum class PlotType : uint8_t {
    Cartesian = 0,
    CartesianParabola,
    CartesianHyperbola,
    Line,
    HorizontalLine,
    // All previous types + Polar and Parametric are active in the values table
    CartesianAlongY,
    VerticalLine,
    // All previous types + Polar and Parametric plot with only one subcurve
    OtherAlongY,
    Circle,
    Ellipse,
    Parabola,
    Hyperbola,
    Other,
    // All previous types are expressions of x and y
    Polar,
    Parametric,
    // All following types shall never be active
    Undefined,
    UndefinedPolar,
    UndefinedParametric,
    Unhandled,
    UnhandledPolar,
    UnhandledParametric,
    Disabled,
    Unknown
  };

  // Return Message corresponding to SymbolType
  static I18n::Message MessageForSymbolType(SymbolType symbolType);
  // If a plotType is an inactive plotType
  static bool IsPlotTypeInactive(PlotType plotType) { return plotType >= PlotType::Undefined; };
  // If a plotType is equivalent to a parabola
  static bool IsPlotTypeParabola(PlotType type) { return type == PlotType::CartesianParabola || type == PlotType::Parabola; }
  // If a plotType is equivalent to an hyperbola
  static bool IsPlotTypeHyperbola(PlotType type) { return type == PlotType::CartesianHyperbola || type == PlotType::Hyperbola; }
  // Return message describing function's PlotType
  I18n::Message plotTypeMessage() const;
  // Return the type of area to draw
  AreaType areaType() const;
  // Return the equation's symbol
  Poincare::ExpressionNode::Type equationType() const { return m_model.equationType(); }
  // Return ContinuousFunction's PlotType. Recompute if Unknown can't be handled
  PlotType plotType() const;
  // Return ContinuousFunction's SymbolType
  SymbolType symbolType() const;

  /* Function */

  // Return Message corresponding to ContinuousFunction's SymbolType
  I18n::Message parameterMessageName() const override { return MessageForSymbolType(symbolType()); }
  // Return CodePoint corresponding to ContinuousFunction's SymbolType
  CodePoint symbol() const override;
  // Return CodePoint corresponding to ContinuousFunction's equation symbol
  CodePoint equationSymbol() const;
  // Insert ContinuousFunction's name and argument in buffer ("f(x)" or "y")
  int nameWithArgument(char * buffer, size_t bufferSize) override;
  // Insert the value of the evaluation (or the symbol if symbolValue) in buffer
  int printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context, bool symbolValue = false) override;
  // Return true if the ContinuousFunction is active
  bool isActive() const override { return Function::isActive() && !IsPlotTypeInactive(plotType()); };

  /* ExpressionModelHandle */

  Ion::Storage::Record::ErrorStatus setContent(const char * c, Poincare::Context * context) override;
  void tidyDownstreamPoolFrom(char * treePoolCursor = nullptr) const override;

  /* Properties */

  // Wether the ContinuousFunction can have a custom domain of definition.
  bool canHaveCustomDomain() const { return !isAlongY() && equationType() == Poincare::ExpressionNode::Type::Equal; }
  // Wether or not we can display the derivative
  bool canDisplayDerivative() const { return isActiveInTable() && isAlongXorY(); }
  // Wether to draw a dotted or solid line (Strict inequalities).
  bool drawDottedCurve() const;
  // If the ContinuousFunction should be considered active in table
  bool isActiveInTable() const;
  /* TODO : ContinuousFunction along y are implemented with a 'x' symbol() so
   * that they share most properties. A better segmentation would allow more
   * complex behaviors such as being active in table or computing derivatives.
   * We don't need these features for now and keep a lighter code. */
  // If the ContinuousFunction has x or y for unknown symbol
  bool isAlongXorY() const { return symbol() == ContinuousFunction::k_cartesianSymbol; }
  // If the ContinuousFunction has y for unknown symbol
  bool isAlongY() const override { return plotType() == PlotType::VerticalLine || plotType() == PlotType::OtherAlongY || plotType() == PlotType::CartesianAlongY; }
  // If the ContinuousFunction is a conic
  bool isConic() const;
  // If the ContinuousFunction is named ("f(x)=...")
  bool isNamed() const;
  /* If we can compute the ContinuousFunction intersections.
   * isAlongY must be false, but it is checked by "isActiveInTable()".
   * TODO : Handle more types of curves ?
   * If intersections are implemented for verticalLines, isActiveInTable might
   * need a change. */
  bool isIntersectable() const { return isActiveInTable() && !(plotType() == PlotType::Polar || plotType() == PlotType::Parametric); }
  bool isDiscontinuousBetweenFloatValues(float x1, float x2, Poincare::Context * context) const;
  // Compute line parameters (slope and intercept) from ContinuousFunction
  void getLineParameters(double * slope, double * intercept, Poincare::Context * context) const;
  // Compute conic parameters from ContinuousFunction
  Poincare::Conic getConicParameters(Poincare::Context * context) const;
  /* Return the number of subcurves to plot.
   * Warning : This function needs the solutions to have been computed in the
   * (expensive) expressionReduced method before yielding a non-zero result.
   * numberOfSubCurves shouldn't be called at stages where the expressionReduced
   * has not been executed yet. */
  int numberOfSubCurves() const override { assert(m_model.numberOfSubCurves() > 0); return m_model.numberOfSubCurves(); }
  int numberOfCurveParameters() const;
  struct CurveParameter {
    I18n::Message parameterName;
    bool editable;
    bool isPreimage;
  };
  CurveParameter getCurveParameter(int index) const;

  /* Expression */

  // Return the unaltered equation expression, replacing unknown symbols.
  Poincare::Expression originalEquation(const Ion::Storage::Record * record) const {
    return m_model.originalEquation(record, symbol());
  }
  // Update plotType as well as tMin and tMax values.
  void updateModel(Poincare::Context * context, bool wasAlongXorY);

  /* Evaluation */

  Poincare::Coordinate2D<double> evaluate2DAtParameter(double t, Poincare::Context * context, int curveIndex = 0) const {
    return templatedApproximateAtParameter(t, context, curveIndex);
  }
  Poincare::Coordinate2D<float> evaluateXYAtParameter(float t, Poincare::Context * context, int curveIndex = 0) const override {
    return m_cache ? m_cache->valueForParameter(this, context, t, curveIndex) : privateEvaluateXYAtParameter<float>(t, context, curveIndex);
  }
  Poincare::Coordinate2D<double> evaluateXYAtParameter(double t, Poincare::Context * context, int curveIndex = 0) const override {
    return privateEvaluateXYAtParameter<double>(t, context, curveIndex);
  }

  double evaluateCurveParameter(int index, double cursorT, double cursorX, double cursorY, Poincare::Context * context) const;

  /* Derivative */

  // If derivative should be displayed
  bool displayDerivative() const { return recordData()->displayDerivative(); }
  // Set derivative display status
  void setDisplayDerivative(bool display) { return recordData()->setDisplayDerivative(display); }
  // Insert derivative name with argument in buffer (f'(x) or y')
  int derivativeNameWithArgument(char * buffer, size_t bufferSize);
  // Approximate derivative at x, on given sub curve if there is one
  double approximateDerivative(double x, Poincare::Context * context, int subCurveIndex = 0) const;

  /* tMin and tMax */

  // tMin getter
  float tMin() const override { return recordData()->tMin(); }
  // tMax getter
  float tMax() const override { return recordData()->tMax(); }
  // tMin setter
  void setTMin(float tMin);
  // tMax setter
  void setTMax(float tMax);

  /* Range */

  // If curve expression is based on costly algorithm for approximation
  bool basedOnCostlyAlgorithms(Poincare::Context * context) const override;
  // Set x range for display adapt y ranges accordingly as well.
  void xRangeForDisplay(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Poincare::Context * context) const override;
  // Set y range for display
  void yRangeForDisplay(float xMin, float xMax, float yMinForced, float yMaxForced, float ratio, float * yMin, float * yMax, Poincare::Context * context, bool optimizeRange) const override;

  /* Extremum */

  // Compute coordinates of the next minimun, from a starting point
  Poincare::Coordinate2D<double> nextMinimumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  // Compute coordinates of the next maximum, from a starting point
  Poincare::Coordinate2D<double> nextMaximumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;

  /* Roots */

  // Compute coordinates of the next root, from a starting point
  Poincare::Coordinate2D<double> nextRootFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  // Compute coordinates of the next intersection, from a starting point
  Poincare::Coordinate2D<double> nextIntersectionFrom(double start, double max, Poincare::Context * context, Poincare::Expression e, double relativePrecision, double minimalStep, double maximalStep, double eDomainMin = -INFINITY, double eDomainMax = INFINITY) const;

  /* Integral */

  // Return the expression of the integral between start and end
  Poincare::Expression sumBetweenBounds(double start, double end, Poincare::Context * context) const override;

  /* Cache */

  ContinuousFunctionCache * cache() const { return m_cache; }
  void setCache(ContinuousFunctionCache * v) { m_cache = v; }
  void didBecomeInactive() override { m_cache = nullptr; }
  constexpr static char k_unnamedRecordFirstChar = '?';
  constexpr static CodePoint k_cartesianSymbol = 'x';
  constexpr static CodePoint k_parametricSymbol = 't';
  constexpr static CodePoint k_polarSymbol = UCodePointGreekSmallLetterTheta;
private:
  constexpr static char k_ordinateName[2] = "y";
  constexpr static CodePoint k_ordinateCodePoint = k_ordinateName[0];
  constexpr static CodePoint k_unnamedExpressionSymbol = k_cartesianSymbol;
  constexpr static float k_polarParamRangeSearchNumberOfPoints = 100.0f; // This is ad hoc, no special justification
  // Units are not handled in the graph app. The default unit does not matters
  constexpr static Poincare::Preferences::UnitFormat k_defaultUnitFormat = Poincare::Preferences::UnitFormat::Metric;

  /* Context */

  static Poincare::Preferences::AngleUnit AngleUnit() { return Poincare::Preferences::sharedPreferences()->angleUnit(); }
  static Poincare::Preferences::ComplexFormat ComplexFormat() { return Poincare::Preferences::sharedPreferences()->complexFormat(); }

  /* Range */

  // Return step computed from t range or NAN if isAlongXorY() is true.
  float rangeStep() const override;

  /* Expressions */

  // Return the expression representing the equation for computations
  Poincare::Expression expressionReducedForAnalysis(Poincare::Context * context) const  {
    return m_model.expressionReducedForAnalysis(this, context);
  }
  // Return reduced curve expression derivative
  Poincare::Expression expressionDerivateReduced(Poincare::Context * context) const { return m_model.expressionDerivateReduced(this, context); }

  /* Evaluation */

  typedef Poincare::Coordinate2D<double> (*ComputePointOfInterest)(Poincare::Expression e, const char * symbol, double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep);
  // Compute coordinates of the next point of interest, from a starting point
  Poincare::Coordinate2D<double> nextPointOfInterestFrom(double start, double max, Poincare::Context * context, ComputePointOfInterest compute, double relativePrecision, double minimalStep, double maximalStep) const;
  // Evaluate XY at parameter (distinct from approximation with Polar types)
  template<typename T> Poincare::Coordinate2D<T> privateEvaluateXYAtParameter(T t, Poincare::Context * context, int subCurveIndex = 0) const;
  // Approximate XY at parameter
  template<typename T> Poincare::Coordinate2D<T> templatedApproximateAtParameter(T t, Poincare::Context * context, int subCurveIndex = 0) const;

  /* Record */

  class __attribute__((packed)) RecordDataBuffer : public Shared::Function::RecordDataBuffer {
  public:
    RecordDataBuffer(KDColor color) :
        Shared::Function::RecordDataBuffer(color),
        m_domain(-INFINITY, INFINITY),
        m_displayDerivative(false) {}
    bool displayDerivative() const { return m_displayDerivative; }
    void setDisplayDerivative(bool display) { m_displayDerivative = display; }
    float tMin() const { return m_domain.min(); }
    float tMax() const { return m_domain.max(); }
    void setTMin(float tMin) { m_domain.setMin(tMin); }
    void setTMax(float tMax) { m_domain.setMax(tMax); }
  private:
    Range1D m_domain;
    bool m_displayDerivative;
    /* In the record, after the boolean flag about displayDerivative, there is
     * the expression of the function, directly copied from the pool. */
    //char m_expression[0];
  };

  // Return metadata size
  size_t metaDataSize() const override { return sizeof(RecordDataBuffer); }
  // Return record data
  RecordDataBuffer * recordData() const {
    assert(!isNull());
    return reinterpret_cast<RecordDataBuffer *>(const_cast<void *>(value().buffer));
  }

  /* Model */

  class Model : public ExpressionModel {
  public:
    Model() :
        ExpressionModel(),
        m_numberOfSubCurves(0),
        m_equationType(Poincare::ExpressionNode::Type::Equal),
        m_plotType(PlotType::Unknown) {}
    // Return the expression to plot.
    Poincare::Expression expressionReduced(const Ion::Storage::Record * record, Poincare::Context * context) const override;
    // Return the expression reduced, and computes plotType
    Poincare::Expression expressionReducedForAnalysis(const Ion::Storage::Record * record, Poincare::Context * context) const;
    // Return the expression of the named function (right side of the equal)
    Poincare::Expression expressionClone(const Ion::Storage::Record * record) const override;
    // Return the entire expression that the user inputted. Replace symbols.
    Poincare::Expression originalEquation(const Ion::Storage::Record * record, CodePoint symbol) const;
    /* Return the expression representing the equation
     * (turns "f(x)=xy" into "xy" and "xy=a" into "xy-a") */
    Poincare::Expression expressionEquation(const Ion::Storage::Record * record, Poincare::Context * context, PlotType * computedPlotType) const;
    // Return the derivative of the expression to plot.
    Poincare::Expression expressionDerivateReduced(const Ion::Storage::Record * record, Poincare::Context * context) const;
    // Rename the record if needed. Record pointer might get corrupted.
    Ion::Storage::Record::ErrorStatus renameRecordIfNeeded(Ion::Storage::Record * record, const char * c, Poincare::Context * context, CodePoint symbol);
    // Build the expression from text, handling f(x)=... cartesian equations
    Poincare::Expression buildExpressionFromText(const char * c, CodePoint symbol = 0, Poincare::Context * context = nullptr) const override;
    // Tidy the model
    void tidyDownstreamPoolFrom(char * treePoolCursor) const override;
    // m_numberOfSubCurves getter
    int numberOfSubCurves() const { return m_numberOfSubCurves; }
    // m_equationType getter
    Poincare::ExpressionNode::Type equationType() const { return m_equationType; }
    // m_plotType getter
    PlotType plotType() const { return m_plotType; }
    // Reset m_plotType to Unknown type
    void resetPlotType() const { m_plotType = PlotType::Unknown; }
    // Update m_plotType depending on the equation
    void updatePlotType(const Ion::Storage::Record * record, const Poincare::Expression equation, Poincare::Context * context) const;
    // If equation has a NonNull coeff. Can also compute last coeff sign.
    static bool HasNonNullCoefficients(const Poincare::Expression equation, const char * symbolName, Poincare::Context * context, Poincare::ExpressionNode::Sign * highestDegreeCoefficientSign);
    // If equation should be allowed when implicit plots are forbidden.
    static bool IsExplicitEquation(const Poincare::Expression equation, CodePoint symbol);
  private:
    // Return address of the record's expression
    void * expressionAddress(const Ion::Storage::Record * record) const override;
    // Return size of the record's expression
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    mutable int m_numberOfSubCurves;
    mutable Poincare::ExpressionNode::Type m_equationType;
    mutable PlotType m_plotType;
    mutable Poincare::Expression m_expressionDerivate;
  };

  // Return model pointer
  const ExpressionModel * model() const override { return &m_model; }

  Model m_model;
  mutable ContinuousFunctionCache * m_cache;
};

}

#endif
