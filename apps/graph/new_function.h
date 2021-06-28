#ifndef GRAPH_NEW_FUNCTION_H
#define GRAPH_NEW_FUNCTION_H

#include "../shared/expression_model_handle.h"
#include "../shared/range_1d.h"
#include <apps/i18n.h>

// TODO Hugo : Emscripten things

namespace Graph {

class NewFunction : public Shared::ExpressionModelHandle {
public:
  static constexpr size_t k_numberOfEquationSymbols = 6;
  enum class EquationSymbol : uint8_t {
    Equal = 0,
    Inequal,
    Greater,
    GreaterOrEqual,
    Less,
    LessOrEqual,
  };

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
    Greater,
    GreaterOrEqual,
    Less,
    LessOrEqual,
    Other,
    Undefined,
    Unhandled
  };

  static NewFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);

  // Properties
  bool isActive() const;
  KDColor color() const;
  void setActive(bool active);
  bool isNamed() const; // y = or f(x) = ?
  bool drawAbove() const; // Greater, GreaterOrEqual, Inequal
  bool drawBelow() const; // Less, LessOrEqual, Inequal
  bool drawCurve() const; // GreaterOrEqual, LessOrEqual, Equal
  int yDegree(Poincare::Context * context) const; // Handled y degree are 0, 1 or 2
  int xDegree(Poincare::Context * context) const; // Any degree is handled

  I18n::Message functionCategory() const; // Line, polar, cartesian, ...
  CodePoint symbol() const override; // x, theta, t, y
  // Expression clone is of the form (exp1) = (exp2)
  // expressionEquation returns (exp1) - (exp2) or (exp2) if isNamed() (reduced ?)
  Poincare::Expression expressionEquation(Poincare::Context * context) const  { return m_model.expressionReduced(this, context); }
  // expressionReduced returns equations solutions in y ( matrix if multiple solutions) // TODO Hugo :: parent implementation should be fine
  // Poincare::Expression expressionReduced(Poincare::Context * context) const override;
  // expressionReduced returns expressionReduced derivative(s)
  // TODO Hugo : Implement
  // Poincare::Expression expressionDerivateReduced(Poincare::Context * context) const { return m_model.expressionDerivateReduced(this, context); }
  EquationSymbol equationSymbol() const { return recordData()->equationSymbol(); }
  PlotType plotType() const { return recordData()->plotType(); }
  void updatePlotType(Poincare::Preferences::AngleUnit angleUnit, Poincare::Context * context);
  static I18n::Message ParameterMessageForPlotType(PlotType plotType);  // x, theta, t, y

  // Evaluation
  Poincare::Coordinate2D<double> evaluate2DAtParameter(double t, Poincare::Context * context) const {
    return templatedApproximateAtParameter(t, context);
  }
  Poincare::Coordinate2D<float> evaluateXYAtParameter(float t, Poincare::Context * context) const {
    // TODO Hugo : cache ?
    return privateEvaluateXYAtParameter<float>(t, context);
  }
  Poincare::Coordinate2D<double> evaluateXYAtParameter(double t, Poincare::Context * context) const {
    return privateEvaluateXYAtParameter<double>(t, context);
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
  void yRangeForDisplay(float xMin, float xMax, float yMinForced, float yMaxForced, float ratio, float * yMin, float * yMax, Poincare::Context * context) const;

  // Extremum
  Poincare::Coordinate2D<double> nextMinimumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  Poincare::Coordinate2D<double> nextMaximumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  // Roots
  Poincare::Coordinate2D<double> nextRootFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  Poincare::Coordinate2D<double> nextIntersectionFrom(double start, double max, Poincare::Context * context, Poincare::Expression e, double relativePrecision, double minimalStep, double maximalStep, double eDomainMin = -INFINITY, double eDomainMax = INFINITY) const;
  // Integral
  Poincare::Expression sumBetweenBounds(double start, double end, Poincare::Context * context) const;
  // TODO Hugo : Consider cache
private:
  // TODO Hugo : usefull ?
  constexpr static float k_polarParamRangeSearchNumberOfPoints = 100.0f; // This is ad hoc, no special justification
  NewFunction(Ion::Storage::Record record = Record()) : Shared::ExpressionModelHandle(record) {}
  typedef Poincare::Coordinate2D<double> (*ComputePointOfInterest)(Poincare::Expression e, char * symbol, double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep);
  Poincare::Coordinate2D<double> nextPointOfInterestFrom(double start, double max, Poincare::Context * context, ComputePointOfInterest compute, double relativePrecision, double minimalStep, double maximalStep) const;
  template <typename T> Poincare::Coordinate2D<T> privateEvaluateXYAtParameter(T t, Poincare::Context * context) const;
  void didBecomeInactive() {} // m_cache = nullptr; }

  void fullXYRange(float * xMin, float * xMax, float * yMin, float * yMax, Poincare::Context * context) const;

  class Model : public Shared::ExpressionModel {
    // TODO Hugo : Add derivative
  public:
    Model() : ExpressionModel() {}
    void tidy() const override;
    Poincare::Expression expressionEquation(const Ion::Storage::Record * record, Poincare::Context * context) const;
    Poincare::Expression expressionReduced(const Ion::Storage::Record * record, Poincare::Context * context) const override;
  // private:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
    size_t expressionSize(const Ion::Storage::Record * record) const override;
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
    EquationSymbol equationSymbol() const { return m_equationSymbol; }
    void setEquationSymbol(EquationSymbol equationSymbol) { m_equationSymbol = equationSymbol; }
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
    EquationSymbol m_equationSymbol;
  };

  Model m_model;
  template<typename T> Poincare::Coordinate2D<T> templatedApproximateAtParameter(T t, Poincare::Context * context) const;
  RecordDataBuffer * recordData() const {
    assert(!isNull());
    Ion::Storage::Record::Data d = value();
    return reinterpret_cast<RecordDataBuffer *>(const_cast<void *>(d.buffer));
  }
};

}

#endif