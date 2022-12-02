#ifndef SHARED_CONTINUOUS_FUNCTION_H
#define SHARED_CONTINUOUS_FUNCTION_H

/* Although the considered functions are not generally continuous
 * mathematically speaking, the present class is named ContinuousFunction to
 * mark the difference with the Sequence class.
 *
 * We could not simply name it Function, since such a class already exists:
 * it is the base class of ContinuousFunction and Sequence.
 */

#include "continuous_function_cache.h"
#include "continuous_function_properties.h"
#include "function.h"
#include "packed_range_1D.h"
#include <apps/apps_container_helper.h>
#include <apps/i18n.h>
#include <poincare/conic.h>
#include <poincare/comparison.h>
#include <poincare/preferences.h>
#include <poincare/symbol_abstract.h>

namespace Shared {

class ContinuousFunction : public Function {
  /* We want the cache to be able to call privateEvaluateXYAtParameter to
   * bypass cache lookup when memoizing the function's values. */
  friend class ContinuousFunctionCache;
public:
  constexpr static int k_maxDefaultNameSize = sizeof("f99");

  // Create a record with baseName
  static ContinuousFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);
  // Builder
  ContinuousFunction(Ion::Storage::Record record = Record()) : Function(record), m_cache(nullptr) {}

  ContinuousFunctionProperties properties() const;

  /* Function */

  CodePoint symbol() const override { return properties().symbol(); }
  // Return Message corresponding to ContinuousFunction's SymbolType
  I18n::Message parameterMessageName() const override { return properties().symbolMessage(); }
  // Insert ContinuousFunction's name and argument in buffer ("f(x)" or "y")
  int nameWithArgument(char * buffer, size_t bufferSize) override;
  // Insert the value of the evaluation (or the symbol if symbolValue) in buffer
  int printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context, bool symbolValue = false) override;
  // Return true if the ContinuousFunction is active
  bool isActive() const override { return Function::isActive() && properties().canBeActive(); };
  // If the ContinuousFunction has y for unknown symbol
  bool isAlongY() const override { return properties().isAlongY(); }

  /* ExpressionModelHandle */

  Ion::Storage::Record::ErrorStatus setContent(const char * c, Poincare::Context * context) override;
  void tidyDownstreamPoolFrom(char * treePoolCursor = nullptr) const override;

  /* Properties */

  // Wether or not we can display the derivative
  bool canDisplayDerivative() const { return isActiveInTable() && properties().isCartesian(); }
  // If the ContinuousFunction should be considered active in table
  bool isActiveInTable() const { return properties().canBeActiveInTable() && isActive(); }
  // If the ContinuousFunction is named ("f(x)=...")
  bool isNamed() const;
  /* If we can compute the ContinuousFunction intersections
   * isAlongY must be false, but it is checked by "isActiveInTable()".
   * TODO : Handle more types of curves ?
   * If intersections are implemented for verticalLines, isActiveInTable might
   * need a change. */
  bool shouldDisplayIntersections() const { return isActiveInTable() && properties().canComputeIntersectionsWithFunctionsAlongSameVariable(); }
  bool isDiscontinuousBetweenFloatValues(float x1, float x2, Poincare::Context * context) const;
  // Compute line parameters (slope and intercept) from ContinuousFunction
  void getLineParameters(double * slope, double * intercept, Poincare::Context * context) const;
  // Compute conic parameters from ContinuousFunction
  Poincare::CartesianConic cartesianConicParameters(Poincare::Context * context) const;
  // Return the number of subcurves to plot.
  int numberOfSubCurves() const override { return m_model.numberOfSubCurves(this); }

  /* Expression */

  // Return the unaltered equation expression, replacing unknown symbols.
  Poincare::Expression originalEquation() const {
    return m_model.originalEquation(this, symbol());
  }
  // Update plotType as well as tMin and tMax values.
  void updateModel(Poincare::Context * context, bool wasCartesian);

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

  /* tMin, tMax and tAuto */

  float tMin() const override { return tAuto() ? autoTMin() : recordData()->tMin(); }
  float tMax() const override { return tAuto() ? autoTMax() : recordData()->tMax(); }
  // If TAuto is true, domain has auto values (which may depend on angle unit)
  bool tAuto() const { return recordData()->tAuto(); }
  void setTMin(float tMin);
  void setTMax(float tMax);
  void setTAuto(bool tAuto);
  float autoTMax() const;
  float autoTMin() const;

  /* Solver */

  // If curve expression is based on costly algorithm for approximation
  bool basedOnCostlyAlgorithms(Poincare::Context * context) const override;
  /* A solver will be run from 'start' to 'end' on this function. Trim 'start'
   * and 'end' to the interval of definition. */
  void trimResolutionInterval(double * start, double * end) const;

  /* Integral */

  // Return the expression of the integral between start and end
  Poincare::Expression sumBetweenBounds(double start, double end, Poincare::Context * context) const override;

  /* Cache */

  ContinuousFunctionCache * cache() const { return m_cache; }
  void setCache(ContinuousFunctionCache * v) { m_cache = v; }
  void didBecomeInactive() override { m_cache = nullptr; }

  constexpr static char k_unnamedRecordFirstChar = '?';
  constexpr static CodePoint k_cartesianSymbol = ContinuousFunctionProperties::k_cartesianSymbol;
  constexpr static CodePoint k_parametricSymbol = ContinuousFunctionProperties::k_parametricSymbol;
  constexpr static CodePoint k_polarSymbol = ContinuousFunctionProperties::k_polarSymbol;
  constexpr static CodePoint k_radiusSymbol = ContinuousFunctionProperties::k_radiusSymbol;
  constexpr static CodePoint k_ordinateSymbol = ContinuousFunctionProperties::k_ordinateSymbol;
  constexpr static CodePoint k_unnamedExpressionSymbol = k_cartesianSymbol;

private:
  constexpr static float k_polarParamRangeSearchNumberOfPoints = 100.0f; // This is ad hoc, no special justification

  /* Range */

  // Return step computed from t range or NAN if properties().isCartesian() is true.
  float rangeStep() const override;

  /* Expressions */

  // Return the expression representing the equation for computations
  Poincare::Expression expressionReducedForAnalysis(Poincare::Context * context) const  {
    return m_model.expressionReducedForAnalysis(this, context);
  }
  // Return reduced curve expression derivative
  Poincare::Expression expressionDerivateReduced(Poincare::Context * context) const { return m_model.expressionDerivateReduced(this, context); }

  /* Evaluation */

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
        m_displayDerivative(false),
        m_tAuto(true) {}
    bool displayDerivative() const { return m_displayDerivative; }
    void setDisplayDerivative(bool display) { m_displayDerivative = display; }
    float tMin() const { assert(!m_tAuto); return m_domain.min(); }
    float tMax() const { assert(!m_tAuto); return m_domain.max(); }
    bool tAuto() const { return m_tAuto; }
    void setTMin(float tMin) { assert(!m_tAuto); m_domain.setMin(tMin); }
    void setTMax(float tMax) { assert(!m_tAuto); m_domain.setMax(tMax); }
    void setTAuto(bool tAuto) { m_tAuto = tAuto; }

  private:
    PackedRange1D m_domain;
    bool m_displayDerivative;
    bool m_tAuto;
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
    // Return the expression to plot.
    Poincare::Expression expressionReduced(const Ion::Storage::Record * record, Poincare::Context * context) const override;
    // Return the expression reduced, and computes plotType
    Poincare::Expression expressionReducedForAnalysis(const Ion::Storage::Record * record, Poincare::Context * context) const;
    // Return the expression of the named function (right side of the equal)
    Poincare::Expression expressionClone(const Ion::Storage::Record * record) const override;
    // Return the entire expression that the user input. Replace symbols.
    Poincare::Expression originalEquation(const Ion::Storage::Record * record, CodePoint symbol) const;
    /* Return the expression representing the equation
     * (turns "f(x)=xy" into "xy" and "xy=a" into "xy-a") */
    Poincare::Expression expressionEquation(const Ion::Storage::Record * record, Poincare::Context * context, Poincare::ComparisonNode::OperatorType * computedEquationType = nullptr, ContinuousFunctionProperties::SymbolType * computedFunctionSymbol = nullptr, bool * isCartesianEquation = nullptr) const;
    // Return the derivative of the expression to plot.
    Poincare::Expression expressionDerivateReduced(const Ion::Storage::Record * record, Poincare::Context * context) const;
    // Rename the record if needed. Record pointer might get corrupted.
    Ion::Storage::Record::ErrorStatus renameRecordIfNeeded(Ion::Storage::Record * record, const char * c, Poincare::Context * context, CodePoint symbol);
    // Build the expression from text, handling f(x)=... cartesian equations
    Poincare::Expression buildExpressionFromText(const char * c, CodePoint symbol = 0, Poincare::Context * context = nullptr) const override;
    // Tidy the model
    void tidyDownstreamPoolFrom(char * treePoolCursor) const override;
    // m_plotType getter
    ContinuousFunctionProperties properties() const { return m_properties; }
    int numberOfSubCurves(const Ion::Storage::Record * record) const;
    // Reset m_plotType to Uninitialized type
    void resetProperties() const { m_properties.reset(); }

  private:
    // Return address of the record's expression
    void * expressionAddress(const Ion::Storage::Record * record) const override;
    // Return size of the record's expression
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    mutable ContinuousFunctionProperties m_properties;
    mutable Poincare::Expression m_expressionDerivate;
  };

  // Return model pointer
  const ExpressionModel * model() const override { return &m_model; }

  Model m_model;
  mutable ContinuousFunctionCache * m_cache;
};

}

#endif
