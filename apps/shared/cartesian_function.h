#ifndef SHARED_CARTESIAN_FUNCTION_H
#define SHARED_CARTESIAN_FUNCTION_H

#include "global_context.h"
#include "function.h"
#include <poincare/symbol.h>

namespace Shared {

class CartesianFunction : public Function {
public:
  static void DefaultName(char buffer[], size_t bufferSize);
  static CartesianFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);
  CartesianFunction(Ion::Storage::Record record = Record()) :
    Function(record)
  {}
  CodePoint symbol() const override;
  Poincare::Expression expressionReduced(Poincare::Context * context) const override;

  enum class PlotType {
    Cartesian = 0,
    Polar = 1,
    Parametric = 2
  };
  PlotType plotType() const;
  void setPlotType(PlotType plotType);

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
  // tMin and tMax
  double tMin() const override;
  double tMax() const override;
  void setTMin(double tMin);
  void setTMax(double tMax);
private:
  template <typename T> Poincare::Coordinate2D<T> privateEvaluateXYAtParameter(T t, Poincare::Context * context) const;
  /* CartesianFunctionRecordDataBuffer is the layout of the data buffer of Record
   * representing a CartesianFunction. See comment on
   * Shared::Function::FunctionRecordDataBuffer about packing. */
#pragma pack(push,1)
  class CartesianFunctionRecordDataBuffer : public FunctionRecordDataBuffer {
  public:
    CartesianFunctionRecordDataBuffer(KDColor color) :
      FunctionRecordDataBuffer(color),
      m_plotType(PlotType::Cartesian),
      m_displayDerivative(false),
      m_tMin(0.0),
      m_tMax(396.0) // TODO LEA RUBEN
    {}
    PlotType plotType() const { return m_plotType; }
    void setPlotType(PlotType plotType) { m_plotType = plotType; }
    bool displayDerivative() const { return m_displayDerivative; }
    void setDisplayDerivative(bool display) { m_displayDerivative = display; }
    double tMin() const { return m_tMin; }
    double tMax() const { return m_tMax; }
    void setTMin(double tMin) { m_tMin = tMin; }
    void setTMax(double tMax) { m_tMax = tMax; }
  private:
    PlotType m_plotType;
    bool m_displayDerivative;
    double m_tMin;
    double m_tMax;
    /* In the record, after the boolean flag about displayDerivative, there is
     * the expression of the function, directly copied from the pool. */
    //char m_expression[0];
  };
#pragma pack(pop)
  class Model : public ExpressionModel {
  public:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
  private:
    size_t expressionSize(const Ion::Storage::Record * record) const override;
  };
  size_t metaDataSize() const override { return sizeof(CartesianFunctionRecordDataBuffer); }
  const ExpressionModel * model() const override { return &m_model; }
  CartesianFunctionRecordDataBuffer * recordData() const;
  template<typename T> Poincare::Coordinate2D<T> templatedApproximateAtParameter(T t, Poincare::Context * context) const;
  Model m_model;
};

}

#endif
