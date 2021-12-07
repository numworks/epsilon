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

  static constexpr int k_maxDefaultNameSize = sizeof("f99");

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

  static constexpr size_t k_numberOfSymbolTypes = 3;
  enum class SymbolType : uint8_t {
    Theta = 0,
    T,
    X
  };

  static constexpr size_t k_numberOfPlotTypes = 19;
  enum class PlotType : uint8_t {
    Cartesian = 0,
    CartesianParabola,
    CartesianHyperbola,
    Line,
    HorizontalLine,
    // All previous types are active in the values table
    VerticalLine,
    // All previous types plot with only one subcurve
    VerticalLines,
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
    Unhandled,
    UnhandledPolar,
    UnhandledParametric,
    Disabled
  };

  // Return Message corresponding to SymbolType
  static I18n::Message MessageForSymbolType(SymbolType symbolType);
  // If the ContinuousFunction is of an inactive plotType
  static bool IsPlotTypeInactive(PlotType plotType) { return plotType >= PlotType::Undefined; };
  // Return message describing function's PlotType
  I18n::Message plotTypeMessage();
  // Return the type of area to draw
  AreaType areaType() const;
  // Return the equation's symbol
  Poincare::ExpressionNode::Type equationType() const { return recordData()->equationType(); }
  // Return ContinuousFunction's PlotType
  PlotType plotType() const { return recordData()->plotType(); }
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

  /* ExpressionModel */

  Ion::Storage::Record::ErrorStatus setContent(const char * c, Poincare::Context * context) override;
  void tidyDownstreamPoolFrom(char * treePoolCursor = nullptr) const override;

  /* Properties */

  // Wether the ContinuousFunction can have a custom domain of definition.
  bool canHaveCustomDomain() const { return !hasVerticalLines() && equationType() == Poincare::ExpressionNode::Type::Equal; }
  // Wether to draw a dotted or solid line (Strict inequalities).
  bool drawDottedCurve() const;
  // If the ContinuousFunction should be considered active in table
  bool isActiveInTable() const { return equationType() == Poincare::ExpressionNode::Type::Equal && plotType() <= PlotType::HorizontalLine && isActive(); }
  // If the ContinuousFunction has x for unknown symbol
  bool isAlongX() const { return symbol() == 'x'; }
  // If the ContinuousFunction is a conic
  bool isConic() const;
  // If the ContinuousFunction is made of vertical lines
  bool hasVerticalLines() const override { return plotType() == PlotType::VerticalLine || plotType() == PlotType::VerticalLines; }
  // If the ContinuousFunction is named ("f(x)=...")
  bool isNamed() const;
  // Compute line parameters (slope and intercept) from ContinuousFunction
  void getLineParameters(double * slope, double * intercept, Poincare::Context * context) const;
  // Compute conic parameters from ContinuousFunction
  Poincare::Conic getConicParameters(Poincare::Context * context) const;
  // Return the number of subcurves to plot
  int numberOfSubCurves() const override { assert(m_model.numberOfSubCurves() > 0); return m_model.numberOfSubCurves(); }

  /* Expression */

  // Return the unaltered equation expression, replacing unknown symbols.
  Poincare::Expression originalEquation(const Ion::Storage::Record * record) const {
    return m_model.originalEquation(record, symbol());
  }
  // Update plotType as well as tMin and tMax values.
  void udpateModel(Poincare::Context * context);

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
  static constexpr char k_unnamedRecordFirstChar = '?';
private:
  static constexpr char k_unknownName[2] = {UCodePointUnknown, 0};
  static constexpr char k_ordinateName[2] = "y";
  static constexpr float k_polarParamRangeSearchNumberOfPoints = 100.0f; // This is ad hoc, no special justification
  static constexpr Poincare::Preferences::UnitFormat k_defaultUnitFormat = Poincare::Preferences::UnitFormat::Metric;
  static constexpr Poincare::ExpressionNode::SymbolicComputation k_defaultSymbolicComputation = Poincare::ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol;

  /* Range */

  // Return step computed from t range or NAN if isAlongX() is true.
  float rangeStep() const override;

  /* Expressions */

  // Return the expression representing the equation for computations
  Poincare::Expression expressionEquation(Poincare::Context * context) const  {
    return m_model.expressionEquation(this, context);
  }
  // Return reduced curve expression derivative
  Poincare::Expression expressionDerivateReduced(Poincare::Context * context) const { return m_model.expressionDerivateReduced(this, context); }

  /* Properties */

  // Update ContinuousFunction's PlotType
  void updatePlotType(Poincare::Context * context);
  // If equation has a NonNull coeff. Can also compute last coeff sign.
  bool hasNonNullCoefficients(Poincare::Expression equation, const char * symbolName, Poincare::Context * context, Poincare::ExpressionNode::Sign * highestDegreeCoefficientSign) const;

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
        m_plotType(PlotType::Undefined),
        m_equationType(Poincare::ExpressionNode::Type::Equal),
        m_displayDerivative(false) {}
    PlotType plotType() const { return m_plotType; }
    void setPlotType(PlotType plotType) { m_plotType = plotType; }
    bool displayDerivative() const { return m_displayDerivative; }
    void setDisplayDerivative(bool display) { m_displayDerivative = display; }
    Poincare::ExpressionNode::Type equationType() const { return m_equationType; }
    void setEquationType(Poincare::ExpressionNode::Type equationType) { m_equationType = equationType; }
    bool isActive() const override { return Shared::Function::RecordDataBuffer::isActive() && !IsPlotTypeInactive(m_plotType); }
    float tMin() const { return m_domain.min(); }
    float tMax() const { return m_domain.max(); }
    void setTMin(float tMin) { m_domain.setMin(tMin); }
    void setTMax(float tMax) { m_domain.setMax(tMax); }
  private:
    Range1D m_domain;
    PlotType m_plotType;
    Poincare::ExpressionNode::Type m_equationType;
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
        m_plotType(PlotType::Undefined),
        m_expressionDerivate() {}
    // Return the expression to plot.
    Poincare::Expression expressionReduced(const Ion::Storage::Record * record, Poincare::Context * context) const override;
    // Return the expression of the named function (right side of the equal)
    Poincare::Expression expressionClone(const Ion::Storage::Record * record) const override;
    // Return the entire expression that the user inputted. Replace symbols.
    Poincare::Expression originalEquation(const Ion::Storage::Record * record, CodePoint symbol) const;
    // Return the expression representing the equation
    Poincare::Expression expressionEquation(const Ion::Storage::Record * record, Poincare::Context * context) const;
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
    // m_equationType setter
    void setEquationType(Poincare::ExpressionNode::Type equationType) const { m_equationType = equationType; }
    // m_plotType getter
    PlotType plotType() const { return m_plotType; }
  private:
    // Return address of the record's expression
    void * expressionAddress(const Ion::Storage::Record * record) const override;
    // Return size of the record's expression
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    mutable int m_numberOfSubCurves;
    mutable Poincare::ExpressionNode::Type m_equationType;
    // TODO Hugo : Improve this m_plotType workaround
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
