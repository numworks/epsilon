#ifndef GRAPH_NEW_FUNCTION_H
#define GRAPH_NEW_FUNCTION_H

#include "../shared/expression_model_handle.h"

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
  int yDegree() const; // Handled y degree are 0, 1 or 2
  int xDegree() const; // Any degree is handled

  I18n::Message functionCategory() const override; // Line, polar, cartesian, ...
  CodePoint symbol() const override; // x, theta, t, y
  // Expression clone is of the form (exp1) = (exp2)
  // expressionEquation returns (exp1) - (exp2) or (exp2) if isNamed() (reduced ?)
  Poincare::Expression expressionEquation(Poincare::Context * context) const;
  // expressionReduced returns equations solutions in y ( matrix if multiple solutions)
  Poincare::Expression expressionReduced(Poincare::Context * context) const override;
  // expressionReduced returns expressionReduced derivative(s)
  Poincare::Expression expressionDerivateReduced(Poincare::Context * context) const { return m_model.expressionDerivateReduced(this, context); }
  EquationSymbol equationSymbol();
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
  bool shouldClipTRangeToXRange() const override;  // Returns true if the function will not be displayed if t is outside x range.
  float tMin() const override;
  float tMax() const override;
  void setTMin(float tMin);
  void setTMax(float tMax);
  float rangeStep() const override;

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
  // TODO Hugo : Consider cache
private:
  NewFunction(Ion::Storage::Record record = Record()) : Shared::ExpressionModelHandle(record) {}
  typedef Poincare::Coordinate2D<double> (*ComputePointOfInterest)(Poincare::Expression e, char * symbol, double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep);
  Poincare::Coordinate2D<double> nextPointOfInterestFrom(double start, double max, Poincare::Context * context, ComputePointOfInterest compute, double relativePrecision, double minimalStep, double maximalStep) const;
  template <typename T> Poincare::Coordinate2D<T> privateEvaluateXYAtParameter(T t, Poincare::Context * context) const;
  void didBecomeInactive() override { m_cache = nullptr; }

  void fullXYRange(float * xMin, float * xMax, float * yMin, float * yMax, Poincare::Context * context) const;

  class Model : public ExpressionModel {
    // TODO Hugo : Add derivative
  public:
    Model() : ExpressionModel() {}
    void tidy() const override;
  private:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
    size_t expressionSize(const Ion::Storage::Record * record) const override;
  };

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
  private:
    uint16_t m_color;
    bool m_active;
    PlotType m_plotType;
    EquationSymbol m_equationSymbol;
  };

  Model m_model;
  RecordDataBuffer * recordData() const;
};

}

#endif


  /* | x  | y  | Status
   * | 0  | 0  | Undefined
   * | 1  | 0  | Vertical Line
   * | 2  | 0  | Unhandled (Multiple veritcal lines ? # TODO)
   * | +  | 0  | Unhandled (Multiple veritcal lines ? # TODO)
   * | 0  | 1  | Horizontal Line
   * | 1  | 1  | Line
   * | 2  | 1  | Cartesian
   * | +  | 1  | Cartesian
   * | 0  | 2  | Unhandled (Multiple horizontal lines ? # TODO)
   * | 1  | 2  | Circle, Ellipsis, Hyperbola, Parabolla # TODO differentiate
   * | 2  | 2  | Circle, Ellipsis, Hyperbola, Parabolla # TODO differentiate
   * | +  | 2  | Unhandled (But could be ? # TODO)
   * | 0  | +  | Unhandled (Multiple horizontal lines ? # TODO)
   * | 1  | +  | Unhandled (Swap x and y ? # TODO)
   * | 2  | +  | Unhandled (Swap x and y ? # TODO)
   * | +  | +  | Unhandled
   *
   *
   * Conic type ?
   * - Both x and y degree 2 ?
   * - One degree 2, one degree 1 : Parabola
   * So sign of x^2 vs sign of y^2 : same : ellipse, different : hyperbola
   * circle : ellipse + same coefficient
   *    No shared coefficient : x^2y or y^2x
   * Parabola, one of them not coeff 2, + additional checks plz
   *    No shared coefficient : xy x^2y or y^2x
   * y^(2) x=-2 y+10 x+x^(2)
   *
   * Ax2+Bxy+Cy2+Dx+Ey+F=0 with A,B,C not all zero
   * Discriminant B^2-4AC :
   *  - Negative, A=C and B=0 : Circle
   *  - Negative : Ellipse
   *  - Null : Parabola
   *  - Positive and A + C = 0 : Rectangular Hyperbola
   *  - Positive : Hyperbola
   *
   * Eccentricity e formula :
   *  1 if parabola, sqrt(...) otherwise
   * Cercle :
   *  r : Rayon :
   *    sqrt(-F/A + (E^2+D^2)/((2A)^2))
   *  (x,y) : Coordonnées du centre :
   *     x = (2CD-BE)/(B^2-4AC)
   *     y = (2AE-BD)/(B^2-4AC)
   *
   * Ellipse :
   *  a : Demi grand axe
   *  b : Demi petit axe
   *  c : Distance centre-foyer
   *  e : excentircité
   *    sqrt(...)
   *  (x,y) : Coordonnées du centre :
   *     x = (2CD-BE)/(B^2-4AC)
   *     y = (2AE-BD)/(B^2-4AC)
   *
   * Hyperbole :
   *  a : Distance centre-sommet
   *  b : Demi axe transverse
   *  c : Distance centre-foyer
   *  e : excentircité
   *    sqrt(...)
   *  (x,y) : Coordonnées du centre :
   *     x = (2CD-BE)/(B^2-4AC)
   *     y = (2AE-BD)/(B^2-4AC)
   *
   * Parabole :
   *  p : Parametre
   *  (x,y) : Coordonnées du sommet :
   *
   *
   */