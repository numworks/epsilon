#ifndef GRAPH_FUNCTION_H
#define GRAPH_FUNCTION_H

#include "expression_model_handle.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Shared {

class NewFunction : public ExpressionModelHandle {
public:
enum class equationSymbol : uint8_t {
    equal = 0,
    inequal,
    greater,
    greaterOrEqual,
    less,
    lessOrEqual,
    }
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
    Undefined,
    Unhandled
  };
  /* 2* means degree exactly 2 with no terms of degree 1. Otherwise, equivalent to +
   * | x  | y  | Status
   * | 0  | 0  | Polar, Parametric or Undefined
   * | 1  | 0  | Veritcal Line
   * | 2  | 0  | Unhandled (Multiple veritcal lines ? # TODO)
   * | +  | 0  | Unhandled (Multiple veritcal lines ? # TODO)
   * | 0  | 1  | Horizontal Line
   * | 1  | 1  | Line
   * | 2  | 1  | Cartesian
   * | +  | 1  | Cartesian
   * | 0  | 2* | Unhandled (Multiple horizontal lines ? # TODO)
   * | 1  | 2* | Parabolla (1 or 1* only ? # TODO)
   * | 2* | 2* | Circle, Ellipsis, Hyperbola
   * | 2  | 2* | Unhandled (But could be ? # TODO)
   * | +  | 2* | Unhandled (But could be ? # TODO)
   * | 2* | 2  | Unhandled (Swap x and y ? # TODO)
   * | 0  | +  | Unhandled (Multiple horizontal lines ? # TODO)
   * | 1  | +  | Unhandled (Swap x and y ? # TODO)
   * | 2* | +  | Unhandled (Swap x and y ? # TODO)
   * | +  | +  | Unhandled
   */
  static ContinuousFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);
  NewFunction(Ion::Storage::Record record = Record()) : ExpressionModelHandle(record) {}

  // Properties
  bool isActive() const;
  KDColor color() const;
  void setActive(bool active);
  bool drawAbove() const; // greater, greaterOrEqual, inequal
  bool drawBelow() const; // less, lessOrEqual, inequal
  bool drawCurve() const; // greaterOrEqual, lessOrEqual, equal
  int yDegree() const; // Handled y degree are 0, 1 or (exactly) 2
  int xDegree() const; //


  I18n::Message functionCategory() const override; // Line, polar, cartesian, ...
  CodePoint symbol() const override; // x, theta, t, y
  Poincare::Expression expressionReduced(Poincare::Context * context) const override;
  Poincare::Expression expressionDerivateReduced(Poincare::Context * context) const { return m_model.expressionDerivateReduced(this, context); }
  PlotType plotType() const;
  void updatePlotType(Poincare::Preferences::AngleUnit angleUnit, Poincare::Context * context);
  static I18n::Message ParameterMessageForPlotType(PlotType plotType);  // x, theta, t, y

  // Evaluation
  Poincare::Coordinate2D<double> evaluate2DAtParameter(double t, Poincare::Context * context) const {
    return templatedApproximateAtParameter(t, context);
  }
  Poincare::Coordinate2D<float> evaluateXYAtParameter(float t, Poincare::Context * context) const override {
    return (m_cache) ? m_cache->valueForParameter(this, context, t) : privateEvaluateXYAtParameter<float>(t, context);
  }
  Poincare::Coordinate2D<double> evaluateXYAtParameter(double t, Poincare::Context * context) const override {
    return privateEvaluateXYAtParameter<double>(t, context);
  }

  // Derivative
  bool displayDerivative() const;
  void setDisplayDerivative(bool display);
  int derivativeNameWithArgument(char * buffer, size_t bufferSize);
  double approximateDerivative(double x, Poincare::Context * context) const;

  int printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context) override;

  // Definition Interval ( signature to plot )
  void protectedFullRangeForDisplay(float tMin, float tMax, float tStep, float * min, float * max, Poincare::Context * context, bool xRange) const;

  // tMin and tMax
  bool shouldClipTRangeToXRange() const override { return plotType() == PlotType::Cartesian; }  // Returns true if the function will not be displayed if t is outside x range.
  float tMin() const override;
  float tMax() const override;
  void setTMin(float tMin);
  void setTMax(float tMax);
  float rangeStep() const override { return plotType() == PlotType::Cartesian ? NAN : (tMax() - tMin())/k_polarParamRangeSearchNumberOfPoints; }

  // Range
  bool basedOnCostlyAlgorithms(Poincare::Context * context) const override;
  void xRangeForDisplay(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Poincare::Context * context) const override;
  void yRangeForDisplay(float xMin, float xMax, float yMinForced, float yMaxForced, float ratio, float * yMin, float * yMax, Poincare::Context * context) const override;

  // Extremum
  Poincare::Coordinate2D<double> nextMinimumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  Poincare::Coordinate2D<double> nextMaximumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  // Roots
  Poincare::Coordinate2D<double> nextRootFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  Poincare::Coordinate2D<double> nextIntersectionFrom(double start, double max, Poincare::Context * context, Poincare::Expression e, double relativePrecision, double minimalStep, double maximalStep, double eDomainMin = -INFINITY, double eDomainMax = INFINITY) const;
  // Integral
  Poincare::Expression sumBetweenBounds(double start, double end, Poincare::Context * context) const override;

  // Cache
  ContinuousFunctionCache * cache() const { return m_cache; }
  void setCache(ContinuousFunctionCache * v) { m_cache = v; }
  Ion::Storage::Record::ErrorStatus setContent(const char * c, Poincare::Context * context) override;
private:
  constexpr static float k_polarParamRangeSearchNumberOfPoints = 100.0f; // This is ad hoc, no special justification
  typedef Poincare::Coordinate2D<double> (*ComputePointOfInterest)(Poincare::Expression e, char * symbol, double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep);
  Poincare::Coordinate2D<double> nextPointOfInterestFrom(double start, double max, Poincare::Context * context, ComputePointOfInterest compute, double relativePrecision, double minimalStep, double maximalStep) const;
  template <typename T> Poincare::Coordinate2D<T> privateEvaluateXYAtParameter(T t, Poincare::Context * context) const;
  void didBecomeInactive() override { m_cache = nullptr; }

  void fullXYRange(float * xMin, float * xMax, float * yMin, float * yMax, Poincare::Context * context) const;

  /* RecordDataBuffer is the layout of the data buffer of Record
   * representing a Function. We want to avoid padding which would:
   * - increase the size of the storage file
   * - introduce junk memory zone which are then crc-ed in Storage::checksum
   *   creating dependency on uninitialized values.
   * - complicate getters, setters and record handling
   * In addition, Record::value() is a pointer to an address inside
   * Ion::Storage::sharedStorage(), and it might be unaligned. We use the packed
   * keyword to warn the compiler that it members are potentially unaligned
   * (otherwise, the compiler can emit instructions that work only on aligned
   * objects). It also solves the padding issue mentioned above.
   */
  class __attribute__((packed)) RecordDataBuffer {
  public:
    RecordDataBuffer(KDColor color) : m_color(color), m_active(true) {}
    KDColor color() const {
      return KDColor::RGB16(m_color);
    }
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
  private:
#if __EMSCRIPTEN__
    /* For emscripten memory representation, loads and stores must be aligned;
     * performing a normal load or store on an unaligned address can fail
     * silently. We thus use 'emscripten_align1_short' type, the unaligned
     * version of uint16_t type to avoid producing an alignment error on the
     * emscripten platform. */
    static_assert(sizeof(emscripten_align1_short) == sizeof(uint16_t), "emscripten_align1_short should have the same size as uint16_t");
    emscripten_align1_short m_color;
#else
    uint16_t m_color;
#endif
    bool m_active;
  };


private:
  RecordDataBuffer * recordData() const;
};

}

#endif
