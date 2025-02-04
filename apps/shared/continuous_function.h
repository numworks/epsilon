#ifndef SHARED_CONTINUOUS_FUNCTION_H
#define SHARED_CONTINUOUS_FUNCTION_H

/* Although the considered functions are not generally continuous
 * mathematically speaking, the present class is named ContinuousFunction to
 * mark the difference with the Sequence class.
 *
 * We could not simply name it Function, since such a class already exists:
 * it is the base class of ContinuousFunction and Sequence.
 */

#include <apps/i18n.h>
#include <poincare/function_properties/conic.h>
#include <poincare/helpers/scatter_plot_iterable.h>
#include <poincare/point_or_scalar.h>
#include <poincare/preferences.h>

#include "continuous_function_cache.h"
#include "continuous_function_properties.h"
#include "function.h"
#include "packed_range_1D.h"

namespace Shared {

class ContinuousFunction : public Function {
  /* We want the cache to be able to call privateEvaluateXYAtParameter to
   * bypass cache lookup when memoizing the function's values. */
  friend class ContinuousFunctionCache;

 public:
  constexpr static int k_maxDefaultNameSize = sizeof("f99");

  // Create a record with baseName
  static ContinuousFunction NewModel(Ion::Storage::Record::ErrorStatus* error,
                                     const char* baseName, KDColor color);
  static bool IsFunctionAssignment(const Poincare::UserExpression e);

  // Builder
  ContinuousFunction(Ion::Storage::Record record = Record());

  ContinuousFunctionProperties properties() const {
    return m_model.properties(this);
  }
  Ion::Storage::Record::ErrorStatus updateNameIfNeeded(
      Poincare::Context* context);

  /* Function */

  CodePoint symbol() const override { return properties().symbol(); }

  size_t nameWithoutArgument(char* buffer, size_t bufferSize,
                             int derivationOrder = 0);
  // Insert ContinuousFunction's name and argument in buffer ("f(x)" or "y")
  size_t nameWithArgument(char* buffer, size_t bufferSize,
                          int derivationOrder = 0) override;
  // Insert the value of the symbol in buffer
  size_t printAbscissaValue(double cursorT, double cursorX, char* buffer,
                            size_t bufferSize, int precision) override;
  // Insert the value of the evaluation in buffer
  size_t printFunctionValue(double cursorT, double cursorX, double cursorY,
                            char* buffer, size_t bufferSize, int precision,
                            Poincare::Context* context) override;
  // Return true if the ContinuousFunction is active
  bool isActive() const override {
    return Function::isActive() && properties().isEnabled();
  };
  // If the ContinuousFunction has y for unknown symbol
  bool isAlongY() const override { return properties().isAlongY(); }

  /* ExpressionModelHandle */

  Ion::Storage::Record::ErrorStatus setContent(
      const Poincare::Layout& l, Poincare::Context* context) override;
  void tidyDownstreamPoolFrom(
      Poincare::PoolObject* treePoolCursor = nullptr) const override;

  /* Properties */

  // Wether or not we can display the derivative
  bool canDisplayDerivative() const {
    return properties().canDisplayDerivative();
  }
  bool canComputeArea() const { return properties().canComputeArea(); }
  bool canPlotDerivatives() const { return properties().canPlotDerivatives(); }
  bool canComputeTangent() const { return properties().canDisplayDerivative(); }
  // If the ContinuousFunction should be considered active in table
  bool isActiveInTable() const {
    return properties().canBeActiveInTable() && isActive();
  }
  // If the ContinuousFunction is named ("f(x)=...")
  bool isNamed() const;
  /* If we can compute the ContinuousFunction intersections
   * isAlongY must be false, but it is checked by "isActiveInTable()".
   * TODO: Handle more types of curves ?
   * If intersections are implemented for verticalLines, isActiveInTable might
   * need a change. */
  bool shouldDisplayIntersections() const {
    return isActiveInTable() &&
           properties().canComputeIntersectionsWithFunctionsAlongSameVariable();
  }
  bool isDiscontinuousOnFloatInterval(float minBound, float maxBound,
                                      Poincare::Context* context) const;
  // Compute line parameters (slope and intercept) from ContinuousFunction
  void getLineParameters(double* slope, double* intercept,
                         Poincare::Context* context) const;
  // Compute conic parameters from ContinuousFunction
  Poincare::CartesianConic cartesianConicParameters(
      Poincare::Context* context) const;
  // Return the number of subcurves to plot.
  int numberOfSubCurves(bool includeDerivatives = false) const override {
    return m_model.numberOfSubCurves(this) +
           includeDerivatives *
               (displayPlotFirstDerivative() + displayPlotSecondDerivative());
  }
  KDColor subCurveColor(int subCurveIndex) const;

  /* Expression */

  // Return the unaltered equation expression, replacing unknown symbols.
  Poincare::UserExpression originalEquation() const {
    return m_model.originalEquation(this, symbol());
  }
  // Update plotType as well as tMin and tMax values.
  void updateModel(Poincare::Context* context, bool wasCartesian);
  Poincare::SystemFunction expressionApproximated(
      Poincare::Context* context, int derivationOrder = 0) const {
    return m_model.expressionApproximated(this, context, derivationOrder);
  }
  Poincare::SystemExpression parametricForm(Poincare::Context* context) const {
    return m_model.parametricForm(this, context);
  }

  /* Evaluation */

  Poincare::Coordinate2D<double> evaluate2DAtParameter(
      double t, Poincare::Context* context, int curveIndex = 0) const {
    return templatedApproximateAtParameter(t, context, curveIndex);
  }
  Poincare::Coordinate2D<float> evaluateXYAtParameter(
      float t, Poincare::Context* context, int curveIndex = 0) const override {
    return m_cache
               ? m_cache->valueForParameter(this, context, t, curveIndex)
               : privateEvaluateXYAtParameter<float>(t, context, curveIndex);
  }
  Poincare::Coordinate2D<double> evaluateXYAtParameter(
      double t, Poincare::Context* context, int curveIndex = 0) const override {
    return privateEvaluateXYAtParameter<double>(t, context, curveIndex);
  }
  template <typename T>
  Poincare::Coordinate2D<T> evaluateXYDerivativeAtParameter(
      T t, Poincare::Context* context, int derivationOrder) const {
    return templatedApproximateAtParameter(
        t, context, subCurveIndexFromDerivationOrder(derivationOrder));
  }

  /* Derivative */

  // If derivative should be displayed
  bool displayValueFirstDerivative() const {
    assert(!recordData()->displayValueFirstDerivative() ||
           canDisplayDerivative());
    return recordData()->displayValueFirstDerivative();
  }
  bool displayPlotFirstDerivative() const {
    assert(!recordData()->displayPlotFirstDerivative() ||
           canDisplayDerivative());
    return recordData()->displayPlotFirstDerivative();
  }
  bool displayValueSecondDerivative() const {
    assert(!recordData()->displayValueSecondDerivative() ||
           canDisplayDerivative());
    return recordData()->displayValueSecondDerivative();
  }
  bool displayPlotSecondDerivative() const {
    assert(!recordData()->displayPlotSecondDerivative() ||
           canDisplayDerivative());
    return recordData()->displayPlotSecondDerivative();
  }
  // Set derivative display status
  void setDisplayValueFirstDerivative(bool display);
  void setDisplayPlotFirstDerivative(bool display);
  void setDisplayValueSecondDerivative(bool display);
  void setDisplayPlotSecondDerivative(bool display);

  enum class DerivativeDisplayType : bool { Plot, Value };
  int derivationOrderFromRelativeIndex(int relativeIndex,
                                       DerivativeDisplayType type) const;
  int derivationOrderFromSubCurveIndex(int subCurveIndex) const override;
  int subCurveIndexFromDerivationOrder(int derivationOrder) const;
  void valuesToDisplayOnDerivativeCurve(int derivationOrder, bool* image,
                                        bool* firstDerivative,
                                        bool* secondDerivative) const;

  // Approximate derivative at t, on given sub curve if there is one
  template <typename T>
  Poincare::PointOrRealScalar<T> approximateDerivative(
      T t, Poincare::Context* context, int derivationOrder = 1,
      bool useDomain = true) const;
  double approximateSlope(double t, Poincare::Context* context) const;

  /* tMin, tMax and tAuto */

  float tMin() const override {
    return tAuto() ? autoTMin() : recordData()->tMin();
  }
  float tMax() const override {
    return tAuto() ? autoTMax() : recordData()->tMax();
  }
  // If TAuto is true, domain has auto values (which may depend on angle unit)
  bool tAuto() const { return recordData()->tAuto(); }
  void setTMin(float tMin);
  void setTMax(float tMax);
  void setTAuto(bool tAuto);
  float autoTMax() const;
  float autoTMin() const;

  /* Solver */

  // If m_expressionApproximated is based on costly algorithm
  bool approximationBasedOnCostlyAlgorithms(Poincare::Context* context) const;
  /* A solver will be run from 'start' to 'end' on this function. Trim 'start'
   * and 'end' to the interval of definition. */
  void trimResolutionInterval(double* start, double* end) const;

  /* Integral */

  // Return the expression of the integral between start and end
  Poincare::UserExpression sumBetweenBounds(
      double start, double end, Poincare::Context* context) const override;

  /* Cache */

  ContinuousFunctionCache* cache() const { return m_cache; }
  void setCache(ContinuousFunctionCache* v) { m_cache = v; }
  void didBecomeInactive() override { m_cache = nullptr; }

  constexpr static char k_unnamedRecordFirstChar = '?';

  /* Scatter plot helper */
  Poincare::ScatterPlotIterable iterateScatterPlot(
      Poincare::Context* context) const {
    assert(properties().isScatterPlot());
    return Poincare::ScatterPlotIterable(expressionReduced(context));
  }

 private:
  constexpr static float k_polarParamRangeSearchNumberOfPoints =
      100.0f;  // This is ad hoc, no special justification

  /* Range */

  /* Return step computed from t range or NAN if properties().isCartesian() is
   * true. */
  float rangeStep() const override;

  /* Expressions */

  // Return the expression representing the equation for computations
  Poincare::SystemExpression expressionReducedForAnalysis(
      Poincare::Context* context) const {
    return m_model.expressionReducedForAnalysis(this, context);
  }
  // Return the reduced expression of the derivative
  Poincare::SystemExpression expressionDerivateReduced(
      Poincare::Context* context, int derivationOrder) const {
    return m_model.expressionDerivateReduced(this, context, derivationOrder);
  }
  // Return the reduced expression of the slope (dy/dx)
  Poincare::SystemFunctionScalar expressionSlopeReduced(
      Poincare::Context* context) const {
    return m_model.expressionSlopeReduced(this, context);
  }

  void updateDerivativeColorAfterChangingPlotDisplay(bool newDisplay,
                                                     int derivationOrder) const;

  /* Evaluation */

  // Evaluate XY at parameter (distinct from approximation with Polar types)
  template <typename T>
  Poincare::Coordinate2D<T> privateEvaluateXYAtParameter(
      T t, Poincare::Context* context, int subCurveIndex = -1) const;
  // Approximate XY at parameter
  template <typename T>
  Poincare::Coordinate2D<T> templatedApproximateAtParameter(
      T t, Poincare::Context* context, int subCurveIndex = -1) const;

  /* Record */

  class __attribute__((packed)) RecordDataBuffer
      : public Shared::Function::RecordDataBuffer {
   public:
    RecordDataBuffer(KDColor color);

    KDColor color(int derivationOrder = 0) const override;
    void setColor(KDColor color, int derivationOrder = 0) override;

    bool displayValueFirstDerivative() const {
      return m_derivativesOptions.valueFirstDerivative;
    }
    bool displayPlotFirstDerivative() const {
      return m_derivativesOptions.plotFirstDerivative;
    }
    bool displayValueSecondDerivative() const {
      return m_derivativesOptions.valueSecondDerivative;
    }
    bool displayPlotSecondDerivative() const {
      return m_derivativesOptions.plotSecondDerivative;
    }
    void setDisplayValueFirstDerivative(bool display) {
      m_derivativesOptions.valueFirstDerivative = display;
    }
    void setDisplayPlotFirstDerivative(bool display) {
      m_derivativesOptions.plotFirstDerivative = display;
    }
    void setDisplayValueSecondDerivative(bool display) {
      m_derivativesOptions.valueSecondDerivative = display;
    }
    void setDisplayPlotSecondDerivative(bool display) {
      m_derivativesOptions.plotSecondDerivative = display;
    }
    float tMin() const {
      assert(!m_tAuto);
      return m_domain.min();
    }
    float tMax() const {
      assert(!m_tAuto);
      return m_domain.max();
    }
    bool tAuto() const { return m_tAuto; }
    void setTMin(float tMin) {
      assert(!m_tAuto);
      m_domain.setMinKeepingValid(tMin);
    }
    void setTMax(float tMax) {
      assert(!m_tAuto);
      m_domain.setMaxKeepingValid(tMax);
    }
    void setTAuto(bool tAuto) { m_tAuto = tAuto; }

   private:
    PackedRange1D m_domain;
    struct DerivativesOptions {
      bool valueFirstDerivative : 1;
      bool plotFirstDerivative : 1;
      bool valueSecondDerivative : 1;
      bool plotSecondDerivative : 1;
#if __EMSCRIPTEN__
      // See comment in function.h
      emscripten_align1_short colorFirstDerivative;
      emscripten_align1_short colorSecondDerivative;
#else
      uint16_t colorFirstDerivative;
      uint16_t colorSecondDerivative;
#endif
    };
    DerivativesOptions m_derivativesOptions;
    bool m_tAuto;
    /* In the record, after the boolean flag about displayValueFirstDerivative,
     * there is the expression of the function, directly copied from the pool.
     */
    // char m_expression[0];
  };

  // Return metadata size
  size_t metaDataSize() const override { return sizeof(RecordDataBuffer); }
  // Return record data
  RecordDataBuffer* recordData() const {
    assert(!isNull());
    return reinterpret_cast<RecordDataBuffer*>(
        const_cast<void*>(value().buffer));
  }

  /* Model */

  class Model : public ExpressionModel {
   public:
    // Return the expression to plot.
    Poincare::SystemExpression expressionReduced(
        const Ion::Storage::Record* record,
        Poincare::Context* context) const override;
    /* Return the expression reduced with approximated non symbols for faster
     * plot */
    Poincare::SystemFunction expressionApproximated(
        const Ion::Storage::Record* record, Poincare::Context* context,
        int derivationOrder = 0) const;
    // Return the expression reduced, and computes plotType
    Poincare::SystemExpression expressionReducedForAnalysis(
        const Ion::Storage::Record* record, Poincare::Context* context) const;
    // Return the expression of the named function (right side of the equal)
    Poincare::UserExpression expressionClone(
        const Ion::Storage::Record* record) const override;
    const Poincare::Internal::Tree* expressionTree(
        const Ion::Storage::Record* record) const override;
    // Return the entire expression that the user input. Replace symbols.
    Poincare::UserExpression originalEquation(
        const Ion::Storage::Record* record, CodePoint symbol) const;
    /* Return the expression representing the equation
     * (turns "f(x)=xy" into "xy" and "xy=a" into "xy-a") */
    Poincare::UserExpression expressionEquation(
        const Ion::Storage::Record* record, Poincare::Context* context,
        Poincare::ComparisonJunior::Operator* computedEquationType = nullptr,
        ContinuousFunctionProperties::SymbolType* computedFunctionSymbol =
            nullptr,
        bool* isCartesianEquation = nullptr) const;
    // Return the derivative of the expression to plot.
    Poincare::SystemExpression expressionDerivateReduced(
        const Ion::Storage::Record* record, Poincare::Context* context,
        int derivationOrder) const;
    // Return the slope (dy/dx)
    Poincare::SystemFunctionScalar expressionSlopeReduced(
        const Ion::Storage::Record* record, Poincare::Context* context) const;
    // Rename the record if needed. Record pointer might get corrupted.
    Ion::Storage::Record::ErrorStatus renameRecordIfNeeded(
        Ion::Storage::Record* record, Poincare::Context* context) const;
    // Build the expression from layout, handling f(x)=... cartesian equations
    Poincare::UserExpression buildExpressionFromLayout(
        Poincare::Layout l, CodePoint symbol = 0,
        Poincare::Context* context = nullptr) const override;
    // Tidy the model
    void tidyDownstreamPoolFrom(
        Poincare::PoolObject* treePoolCursor) const override;
    // m_plotType getter
    ContinuousFunctionProperties properties(
        const Ion::Storage::Record* record) const;
    int numberOfSubCurves(const Ion::Storage::Record* record) const;
    // Reset m_plotType to Uninitialized type
    void resetProperties() const { m_properties.reset(); }
    Poincare::SystemExpression parametricForm(
        const Ion::Storage::Record* record, Poincare::Context* context) const;

   private:
    static CodePoint CodePointForSymbol(const Poincare::UserExpression& symbol);

    // Return address of the record's expression
    void* expressionAddress(const Ion::Storage::Record* record) const override;
    // Return size of the record's expression
    size_t expressionSize(const Ion::Storage::Record* record) const override;

    void setStorageChangeFlag() const override;

    mutable ContinuousFunctionProperties m_properties;
    /* m_expression is used for values in table.
     * m_expressionApproximated is used for plot, autozoom and points of
     * interest.
     */
    mutable Poincare::SystemFunction m_expressionApproximated;
    mutable Poincare::SystemExpression m_expressionFirstDerivate;
    mutable Poincare::SystemFunction m_expressionFirstDerivateApproximated;
    mutable Poincare::SystemExpression m_expressionSecondDerivate;
    mutable Poincare::SystemFunction m_expressionSecondDerivateApproximated;
    mutable Poincare::SystemFunctionScalar m_expressionSlope;
  };

  // Return model pointer
  const ExpressionModel* model() const override { return &m_model; }

  Model m_model;
  mutable ContinuousFunctionCache* m_cache;
};

}  // namespace Shared

#endif
