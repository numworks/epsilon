#ifndef GRAPH_NEW_FUNCTION_H
#define GRAPH_NEW_FUNCTION_H

#include "../shared/expression_model_handle.h"
#include "../shared/range_1D.h"
#include <apps/i18n.h>
#include <poincare/symbol_abstract.h>
#include <poincare/conic.h>

// TODO Hugo : Emscripten things

namespace Graph {

class NewFunction : public Shared::ExpressionModelHandle {
public:
  /* Possible arguments: n, x, t, θ
   * The CodePoint θ is two char long. */
  constexpr static int k_parenthesedArgumentCodePointLength = 3;
  constexpr static int k_parenthesedThetaArgumentByteLength = 4;
  constexpr static int k_parenthesedXNTArgumentByteLength = 3;
  constexpr static int k_maxNameWithArgumentSize = Poincare::SymbolAbstract::k_maxNameSize + k_parenthesedThetaArgumentByteLength; /* Function name and null-terminating char + "(θ)" */;

  static constexpr size_t k_numberOfPlotTypes = 20;
  enum class PlotType : uint8_t {
    Cartesian = 0,
    Polar,
    Parametric,
    Line,
    VerticalLine,
    HorizontalLine,
    Inequation,
    Conics,
    Circle,
    Ellipse,
    Parabola,
    Hyperbola,
    Inequality,
    Superior,
    SuperiorEqual,
    Inferior,
    InferiorEqual,
    Other,
    Undefined,
    Unhandled
  };

  static NewFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);
  NewFunction(Ion::Storage::Record record = Record()) : Shared::ExpressionModelHandle(record) {}

  // Properties
  bool isActive() const;
  KDColor color() const;
  void setActive(bool active);
  bool isNamed() const; // y = or f(x) = ?
  bool isAlongX() const { return symbol() == 'x'; }
  bool hasTwoCurves() const { return m_model.hasTwoCurves(); }
  bool drawSuperiorArea() const; // Superior, SuperiorEqual, Inequal
  bool drawInferiorArea() const; // Inferior, InferiorEqual, Inequal
  bool drawCurve() const; // SuperiorEqual, InferiorOrEqual, Equal
  int yDegree(Poincare::Context * context) const; // Handled y degree are 0, 1 or 2
  int xDegree(Poincare::Context * context) const; // Any degree is handled

  int nameWithArgument(char * buffer, size_t bufferSize);
  I18n::Message parameterMessageName() const;

  I18n::Message functionCategory() const; // Line, polar, cartesian, ...

  int detailsTotal() const;
  I18n::Message detailsTitle(int i) const;
  I18n::Message detailsDescription(int i) const;
  double detailsValue(int i) const;

  CodePoint symbol() const override; // x, theta, t, y
  // Expression clone is of the form (exp1) = (exp2)
  // expressionEquation returns (exp1) - (exp2) or (exp2) if isNamed() (reduced ?)
  Poincare::Expression expressionEquation(Poincare::Context * context) const  { return m_model.expressionEquation(this, context); }
  // expressionReduced returns equations solutions in y ( matrix if multiple solutions) // TODO Hugo :: parent implementation should be fine
  // Poincare::Expression expressionReduced(Poincare::Context * context) const override;
  // expressionReduced returns expressionReduced derivative(s)
  // TODO Hugo : Implement
  // Poincare::Expression expressionDerivateReduced(Poincare::Context * context) const { return m_model.expressionDerivateReduced(this, context); }
  Poincare::ExpressionNode::Type equationSymbol() const { return recordData()->equationSymbol(); }
  PlotType plotType() const { return recordData()->plotType(); }
  void updatePlotType(Poincare::Preferences::AngleUnit angleUnit, Poincare::Context * context);
  static I18n::Message ParameterMessageForPlotType(PlotType plotType);  // x, theta, t, y

  // Evaluation
  Poincare::Coordinate2D<double> evaluate2DAtParameter(double t, Poincare::Context * context, int i = 0) const {
    return templatedApproximateAtParameter(t, context, i);
  }
  Poincare::Coordinate2D<float> evaluateXYAtParameter(float t, Poincare::Context * context, int i = 0) const {
    // TODO Hugo : cache ?
    return privateEvaluateXYAtParameter<float>(t, context, i);
  }
  Poincare::Coordinate2D<double> evaluateXYAtParameter(double t, Poincare::Context * context, int i = 0) const {
    return privateEvaluateXYAtParameter<double>(t, context, i);
  }

  // Derivative
  bool displayDerivative() const;
  void setDisplayDerivative(bool display);
  int derivativeNameWithArgument(char * buffer, size_t bufferSize);
  double approximateDerivative(double x, Poincare::Context * context) const;

  int printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context);

  // Definition Interval ( signature to plot )
  void protectedFullRangeForDisplay(float tMin, float tMax, float tStep, float * min, float * max, Poincare::Context * context, bool xRange) const;

  // tMin and tMax
  bool shouldClipTRangeToXRange() const;  // Returns true if the function will not be displayed if t is outside x range.
  float tMin() const;
  float tMax() const;
  void setTMin(float tMin);
  void setTMax(float tMax);
  float rangeStep() const;

  // Range
  bool basedOnCostlyAlgorithms(Poincare::Context * context) const;
  void xRangeForDisplay(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Poincare::Context * context) const;
  void yRangeForDisplay(float xMin, float xMax, float yMinForced, float yMaxForced, float ratio, float * yMin, float * yMax, Poincare::Context * context, bool optimizeRange) const;

  // Extremum
  Poincare::Coordinate2D<double> nextMinimumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  Poincare::Coordinate2D<double> nextMaximumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  // Roots
  Poincare::Coordinate2D<double> nextRootFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  Poincare::Coordinate2D<double> nextIntersectionFrom(double start, double max, Poincare::Context * context, Poincare::Expression e, double relativePrecision, double minimalStep, double maximalStep, double eDomainMin = -INFINITY, double eDomainMax = INFINITY) const;
  // Integral
  Poincare::Expression sumBetweenBounds(double start, double end, Poincare::Context * context) const;
  // TODO Hugo : Consider cache
  Ion::Storage::Record::ErrorStatus setContent(const char * c, Poincare::Context * context) override;
private:
  // TODO Hugo : Improve that
  static Poincare::Conic s_tempConic;
  // TODO Hugo : usefull ?
  constexpr static float k_polarParamRangeSearchNumberOfPoints = 100.0f; // This is ad hoc, no special justification
  typedef Poincare::Coordinate2D<double> (*ComputePointOfInterest)(Poincare::Expression e, char * symbol, double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep);
  Poincare::Coordinate2D<double> nextPointOfInterestFrom(double start, double max, Poincare::Context * context, ComputePointOfInterest compute, double relativePrecision, double minimalStep, double maximalStep) const;
  template <typename T> Poincare::Coordinate2D<T> privateEvaluateXYAtParameter(T t, Poincare::Context * context, int i = 0) const;
  void didBecomeInactive() {} // m_cache = nullptr; }

  void fullXYRange(float * xMin, float * xMax, float * yMin, float * yMax, Poincare::Context * context) const;

  class Model : public Shared::ExpressionModel {
    // TODO Hugo : Add derivative
  public:
    Model() : ExpressionModel(), m_hasTwoCurves(false), m_equationSymbol(Poincare::ExpressionNode::Type::Equal) {}
    Poincare::Expression expressionEquation(const Ion::Storage::Record * record, Poincare::Context * context) const;
    Poincare::Expression expressionReduced(const Ion::Storage::Record * record, Poincare::Context * context) const override;
    bool hasTwoCurves() const { return m_hasTwoCurves; }
  // private:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    void updateNewDataWithExpression(Ion::Storage::Record * record, const Poincare::Expression & expressionToStore, void * expressionAddress, size_t expressionToStoreSize, size_t previousExpressionSize) override;
    mutable bool m_hasTwoCurves;
    mutable Poincare::ExpressionNode::Type m_equationSymbol;
  };
  size_t metaDataSize() const override { return sizeof(RecordDataBuffer); }
  const Shared::ExpressionModel * model() const override { return &m_model; }

  // TODO Hugo : Padding
  class __attribute__((packed)) RecordDataBuffer {
  public:
    RecordDataBuffer(KDColor color) : m_color(color), m_active(true) {}
    KDColor color() const {
      return KDColor::RGB16(m_color);
    }
    PlotType plotType() const { return m_plotType; }
    void setPlotType(PlotType plotType) { m_plotType = plotType; }
    Poincare::ExpressionNode::Type equationSymbol() const { return m_equationSymbol; }
    void setEquationSymbol(Poincare::ExpressionNode::Type equationSymbol) { m_equationSymbol = equationSymbol; }
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
    float tMin() const { return m_domain.min(); }
    float tMax() const { return m_domain.max(); }
    void setTMin(float tMin) { m_domain.setMin(tMin); }
    void setTMax(float tMax) { m_domain.setMax(tMax); }
  private:
    Shared::Range1D m_domain;
    uint16_t m_color;
    bool m_active;
    PlotType m_plotType;
    Poincare::ExpressionNode::Type m_equationSymbol;
  };

  Model m_model;
  template<typename T> Poincare::Coordinate2D<T> templatedApproximateAtParameter(T t, Poincare::Context * context, int i = 0) const;
  RecordDataBuffer * recordData() const {
    assert(!isNull());
    Ion::Storage::Record::Data d = value();
    return reinterpret_cast<RecordDataBuffer *>(const_cast<void *>(d.buffer));
  }
};

}

#endif