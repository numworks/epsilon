#ifndef SHARED_CONTINUOUS_FUNCTION_H
#define SHARED_CONTINUOUS_FUNCTION_H

/* Although the considered functions are not generally continuous
 * mathematically speaking, the present class is named ContinuousFunction to
 * mark the difference with the Sequence class.
 *
 * We could not simply name it Function, since such a class already exists:
 * it is the base class of ContinuousFunction and Sequence.
 */

#include "global_context.h"
#include "function.h"
#include "range_1D.h"
#include <poincare/symbol.h>
#include <poincare/coordinate_2D.h>

namespace Shared {

class ContinuousFunction : public Function {
public:
  static void DefaultName(char buffer[], size_t bufferSize);
  static ContinuousFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);
  ContinuousFunction(Ion::Storage::Record record = Record()) :
    Function(record)
  {}
  I18n::Message parameterMessageName() const override;
  CodePoint symbol() const override;
  Poincare::Expression expressionReduced(Poincare::Context * context) const override;

  static constexpr int k_numberOfPlotTypes = 3;
  enum class PlotType : uint8_t {
    Cartesian = 0,
    Polar = 1,
    Parametric = 2
  };
  PlotType plotType() const;
  void setPlotType(PlotType plotType, Poincare::Preferences::AngleUnit angleUnit, Poincare::Context * context);
  static I18n::Message ParameterMessageForPlotType(PlotType plotType);

  // Evaluation
  Poincare::Coordinate2D<double> evaluate2DAtParameter(double t, Poincare::Context * context) const {
    return templatedApproximateAtParameter(t, context);
  }
  Poincare::Coordinate2D<float> evaluateXYAtParameter(float t, Poincare::Context * context) const override {
    return privateEvaluateXYAtParameter<float>(t, context);
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

  // tMin and tMax
  bool shouldClipTRangeToXRange() const override { return plotType() == PlotType::Cartesian; }
  float tMin() const override;
  float tMax() const override;
  void setTMin(float tMin);
  void setTMax(float tMax);
  float rangeStep() const override { return plotType() == PlotType::Cartesian ? NAN : (tMax() - tMin())/k_polarParamRangeSearchNumberOfPoints; }

  // Extremum
  Poincare::Coordinate2D<double> nextMinimumFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Coordinate2D<double> nextMaximumFrom(double start, double step, double max, Poincare::Context * context) const;
  // Roots
  Poincare::Coordinate2D<double> nextRootFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Coordinate2D<double> nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, Poincare::Expression e, double eDomainMin = -INFINITY, double eDomainMax = INFINITY) const;
  // Integral
  Poincare::Expression sumBetweenBounds(double start, double end, Poincare::Context * context) const override;
private:
  constexpr static float k_polarParamRangeSearchNumberOfPoints = 100.0f; // This is ad hoc, no special justification
  typedef Poincare::Coordinate2D<double> (*ComputePointOfInterest)(Poincare::Expression e, char * symbol, double start, double step, double max, Poincare::Context * context);
  Poincare::Coordinate2D<double> nextPointOfInterestFrom(double start, double step, double max, Poincare::Context * context, ComputePointOfInterest compute) const;
  template <typename T> Poincare::Coordinate2D<T> privateEvaluateXYAtParameter(T t, Poincare::Context * context) const;
  /* RecordDataBuffer is the layout of the data buffer of Record
   * representing a ContinuousFunction. See comment on
   * Shared::Function::RecordDataBuffer about packing. */
  class __attribute__((packed)) RecordDataBuffer : public Function::RecordDataBuffer {
  public:
    RecordDataBuffer(KDColor color) :
      Function::RecordDataBuffer(color),
      m_plotType(PlotType::Cartesian),
      m_domain(-INFINITY, INFINITY),
      m_displayDerivative(false)
    {}
    PlotType plotType() const { return m_plotType; }
    void setPlotType(PlotType plotType) { m_plotType = plotType; }
    bool displayDerivative() const { return m_displayDerivative; }
    void setDisplayDerivative(bool display) { m_displayDerivative = display; }
    float tMin() const { return m_domain.min(); }
    float tMax() const { return m_domain.max(); }
    void setTMin(float tMin) { m_domain.setMin(tMin); }
    void setTMax(float tMax) { m_domain.setMax(tMax); }
  private:
    PlotType m_plotType;
    Range1D m_domain;
    bool m_displayDerivative;
    /* In the record, after the boolean flag about displayDerivative, there is
     * the expression of the function, directly copied from the pool. */
    //char m_expression[0];
  };
  class Model : public ExpressionModel {
  private:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
    size_t expressionSize(const Ion::Storage::Record * record) const override;
  };
  size_t metaDataSize() const override { return sizeof(RecordDataBuffer); }
  const ExpressionModel * model() const override { return &m_model; }
  RecordDataBuffer * recordData() const;
  template<typename T> Poincare::Coordinate2D<T> templatedApproximateAtParameter(T t, Poincare::Context * context) const;
  Model m_model;
};

}

#endif
