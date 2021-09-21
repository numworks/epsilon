#ifndef SHARED_CONTINUOUS_FUNCTION_H
#define SHARED_CONTINUOUS_FUNCTION_H

#include "function.h"
#include "range_1D.h"
#include <apps/i18n.h>
#include <poincare/symbol_abstract.h>
#include <poincare/conic.h>

// TODO Hugo : Emscripten things

namespace Shared {

class ContinuousFunction : public Function {
public:
  // x, t, θ
  static constexpr size_t k_numberOfSymbolTypes = 3;
  enum class SymbolType : uint8_t {
    Theta = 0,
    T,
    X,
  };

  static constexpr size_t k_numberOfPlotTypes = 14;
  enum class PlotType : uint8_t {
    Polar = 0,
    Parametric,
    // All remaining types use x as a symbol
    Cartesian,
    Line,
    HorizontalLine,
    // All remaining types shall not be active in values table
    VerticalLine,
    Inequation,
    Circle,
    Ellipse,
    Parabola,
    Hyperbola,
    Other,
    // All remaining types shall not be active
    Undefined,
    Unhandled
  };

  // TODO Hugo : Add staticAssert for this
  static SymbolType SymbolForPlotType(PlotType plotType) {
    return plotType >= PlotType::Cartesian ? SymbolType::X : static_cast<SymbolType>(plotType);
  }

  static ContinuousFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);
  ContinuousFunction(Ion::Storage::Record record = Record()) : Function(record) {}

  // Properties
  // TODO Hugo : Add staticAssert for this
  bool isActiveInTable() const { return plotType() < PlotType::VerticalLine && isActive(); }
  bool isNamed() const; // y = or f(x) = ?
  bool isAlongX() const { return symbol() == 'x'; }
  bool hasTwoCurves() const { return m_model.hasTwoCurves(); }
  bool drawSuperiorArea() const; // Superior, SuperiorEqual
  bool drawInferiorArea() const; // Inferior, InferiorEqual
  bool drawCurve() const; // SuperiorEqual, InferiorOrEqual, Equal
  int yDegree(Poincare::Context * context) const; // Handled y degree are 0, 1 or 2
  int xDegree(Poincare::Context * context) const; // Any degree is handled
  bool isYCoefficientNonNull(int yDeg, Poincare::Context * context, Poincare::ExpressionNode::Sign * YSign = nullptr) const;

  int nameWithArgument(char * buffer, size_t bufferSize) override;
  I18n::Message parameterMessageName() const override;

  I18n::Message functionCategory() const; // Line, polar, cartesian, ...

  int detailsTotal() const;
  I18n::Message detailsTitle(int i) const;
  I18n::Message detailsDescription(int i) const;
  double detailsValue(int i) const;

  CodePoint symbol() const override; // x, theta, t, y
  // Expression clone is of the form (exp1) = (exp2)
  // expressionEquation returns (exp1) - (exp2) or (exp2) if isNamed() (reduced ?)
  Poincare::Expression expressionEquation(Poincare::Context * context) const  { return m_model.expressionEquation(this, context); }
  // expressionReduced returns expressionReduced derivative(s)
  Poincare::Expression expressionDerivateReduced(Poincare::Context * context) const { return m_model.expressionDerivateReduced(this, context); }
  Poincare::ExpressionNode::Type equationSymbol() const { return recordData()->equationSymbol(); }
  Poincare::Expression equationExpression(const Ion::Storage::Record * record) const {
    return m_model.originalEquation(record, symbol());
  }
  PlotType plotType() const { return recordData()->plotType(); }
  SymbolType symbolType() const { return SymbolForPlotType(plotType()); }
  void updatePlotType(Poincare::Preferences::AngleUnit angleUnit, Poincare::Context * context);
  static I18n::Message ParameterMessageForSymbolType(SymbolType symbolType);  // x, theta, t, y

  // Evaluation
  Poincare::Coordinate2D<double> evaluate2DAtParameter(double t, Poincare::Context * context, int i = 0) const {
    return templatedApproximateAtParameter(t, context, i);
  }
  Poincare::Coordinate2D<float> evaluateXYAtParameter(float t, Poincare::Context * context, int i = 0) const override {
    // TODO Hugo : cache ?
    return privateEvaluateXYAtParameter<float>(t, context, i);
  }
  Poincare::Coordinate2D<double> evaluateXYAtParameter(double t, Poincare::Context * context, int i = 0) const override {
    return privateEvaluateXYAtParameter<double>(t, context, i);
  }

  // Derivative
  bool displayDerivative() const;
  void setDisplayDerivative(bool display);
  int derivativeNameWithArgument(char * buffer, size_t bufferSize);
  double approximateDerivative(double x, Poincare::Context * context, int i = 0) const;

  int printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context) override;

  // tMin and tMax
  bool shouldClipTRangeToXRange() const override;  // Returns true if the function will not be displayed if t is outside x range.
  float tMin() const override;
  float tMax() const override;
  void setTMin(float tMin);
  void setTMax(float tMax);
  float rangeStep() const override;

  // Range
  bool basedOnCostlyAlgorithms(Poincare::Context * context) const override;
  void xRangeForDisplay(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Poincare::Context * context) const override;
  void yRangeForDisplay(float xMin, float xMax, float yMinForced, float yMaxForced, float ratio, float * yMin, float * yMax, Poincare::Context * context, bool optimizeRange) const override;

  // Extremum
  Poincare::Coordinate2D<double> nextMinimumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  Poincare::Coordinate2D<double> nextMaximumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  // Roots
  Poincare::Coordinate2D<double> nextRootFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  Poincare::Coordinate2D<double> nextIntersectionFrom(double start, double max, Poincare::Context * context, Poincare::Expression e, double relativePrecision, double minimalStep, double maximalStep, double eDomainMin = -INFINITY, double eDomainMax = INFINITY) const;
  // Integral
  Poincare::Expression sumBetweenBounds(double start, double end, Poincare::Context * context) const override;
  // TODO Hugo : Consider cache
  Ion::Storage::Record::ErrorStatus setContent(const char * c, Poincare::Context * context) override;
  void udpateModel(Poincare::Context * context);
private:
  static constexpr char k_unknownName[2] = {UCodePointUnknown, 0};
  static constexpr char k_ordinateName[2] = "y";
  // TODO Hugo : usefull ?
  static constexpr float k_polarParamRangeSearchNumberOfPoints = 100.0f; // This is ad hoc, no special justification
  typedef Poincare::Coordinate2D<double> (*ComputePointOfInterest)(Poincare::Expression e, char * symbol, double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep);
  Poincare::Coordinate2D<double> nextPointOfInterestFrom(double start, double max, Poincare::Context * context, ComputePointOfInterest compute, double relativePrecision, double minimalStep, double maximalStep) const;
  template <typename T> Poincare::Coordinate2D<T> privateEvaluateXYAtParameter(T t, Poincare::Context * context, int i = 0) const;
  // TODO Hugo : Restore cache or remove it
  void didBecomeInactive() override {} // m_cache = nullptr; }

  class Model : public ExpressionModel {
  public:
    Model() : ExpressionModel(), m_hasTwoCurves(false), m_equationSymbol(Poincare::ExpressionNode::Type::Equal), m_plotType(PlotType::Undefined), m_expressionDerivate() {}
    // TODO Hugo : Properly rename these functions
    Poincare::Expression originalEquation(const Ion::Storage::Record * record, CodePoint symbol) const;
    Poincare::Expression expressionEquation(const Ion::Storage::Record * record, Poincare::Context * context) const;
    Poincare::Expression expressionReduced(const Ion::Storage::Record * record, Poincare::Context * context) const override;
    Poincare::Expression expressionClone(const Ion::Storage::Record * record) const override;
    Poincare::Expression expressionDerivateReduced(const Ion::Storage::Record * record, Poincare::Context * context) const;
    void tidy() const override;
    bool hasTwoCurves() const { return m_hasTwoCurves; }
  // private:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    mutable bool m_hasTwoCurves;
    mutable Poincare::ExpressionNode::Type m_equationSymbol;
    // TODO Hugo : Avoid this
    mutable PlotType m_plotType;
    mutable Poincare::Expression m_expressionDerivate;
  };
  size_t metaDataSize() const override { return sizeof(RecordDataBuffer); }
  const ExpressionModel * model() const override { return &m_model; }

  // TODO Hugo : Padding
  class __attribute__((packed)) RecordDataBuffer : public Shared::Function::RecordDataBuffer {
  public:
    RecordDataBuffer(KDColor color) : Shared::Function::RecordDataBuffer(color), m_domain(-INFINITY, INFINITY), m_displayDerivative(false), m_plotType(PlotType::Undefined), m_equationSymbol(Poincare::ExpressionNode::Type::Equal)  {}
    PlotType plotType() const { return m_plotType; }
    void setPlotType(PlotType plotType) { m_plotType = plotType; }
    bool displayDerivative() const { return m_displayDerivative; }
    void setDisplayDerivative(bool display) { m_displayDerivative = display; }
    Poincare::ExpressionNode::Type equationSymbol() const { return m_equationSymbol; }
    void setEquationSymbol(Poincare::ExpressionNode::Type equationSymbol) { m_equationSymbol = equationSymbol; }
    // TODO Hugo : Fix Turn on/off menu
    bool isActive() const override { return Shared::Function::RecordDataBuffer::isActive() && m_plotType != PlotType::Unhandled && m_plotType != PlotType::Undefined; }
    float tMin() const { return m_domain.min(); }
    float tMax() const { return m_domain.max(); }
    void setTMin(float tMin) { m_domain.setMin(tMin); }
    void setTMax(float tMax) { m_domain.setMax(tMax); }
    void setConic(Poincare::Conic conic) { m_conic = conic; }
    Poincare::Conic getConic() const { return m_conic; }
    void setLine(double a, double b) { m_lineA = a; m_lineB = b; }
    double getLine(size_t paramIndex) const { assert(paramIndex < 2); return paramIndex == 0 ? m_lineA : m_lineB; }
  private:
    // sizeof - align
    // TODO Hugo : Use Emscripten types for packed alignment
    Poincare::Conic m_conic; // 80 - 8
    double m_lineA; // 8 - 8
    double m_lineB; // 8 - 8
    Range1D m_domain; // 8 - 4
    bool m_displayDerivative; // 1 - 1
    PlotType m_plotType; // 1 - 1
    Poincare::ExpressionNode::Type m_equationSymbol; // 1 - 1
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
