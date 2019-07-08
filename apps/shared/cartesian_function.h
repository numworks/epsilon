#ifndef SHARED_CARTESIAN_FUNCTION_H
#define SHARED_CARTESIAN_FUNCTION_H

#include "global_context.h"
#include "function.h"
#include <poincare/symbol.h>

namespace Shared {

class CartesianFunction : public Function {
public:
  static void DefaultName(char buffer[], size_t bufferSize);
  static CodePoint Symbol() { return 'x'; }
  static CartesianFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);
  CartesianFunction(Ion::Storage::Record record = Record()) :
    Function(record)
  {}
  CodePoint symbol() const override { return 'x'; }
  CodePoint unknownSymbol() const override { return UCodePointUnknownX; }
  Ion::Storage::Record::ErrorStatus setContent(const char * c) override { return editableModel()->setContent(this, c, Symbol(), UCodePointUnknownX); }

  // Derivative
  bool displayDerivative() const;
  void setDisplayDerivative(bool display);
  int derivativeNameWithArgument(char * buffer, size_t bufferSize, CodePoint arg);
  double approximateDerivative(double x, Poincare::Context * context) const;
  // Integral
  double sumBetweenBounds(double start, double end, Poincare::Context * context) const override;
  // Extremum
  Poincare::Coordinate2D nextMinimumFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Coordinate2D nextMaximumFrom(double start, double step, double max, Poincare::Context * context) const;
  // Roots
  double nextRootFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Coordinate2D nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, Poincare::Expression expression) const;
private:
  /* CartesianFunctionRecordDataBuffer is the layout of the data buffer of Record
   * representing a CartesianFunction. See comment on
   * Shared::Function::FunctionRecordDataBuffer about packing. */
#pragma pack(push,1)
  class CartesianFunctionRecordDataBuffer : public FunctionRecordDataBuffer {
  public:
    CartesianFunctionRecordDataBuffer(KDColor color) :
      FunctionRecordDataBuffer(color),
      m_displayDerivative(false)
    {}
    bool displayDerivative() const { return m_displayDerivative; }
    void setDisplayDerivative(bool display) { m_displayDerivative = display; }
  private:
    bool m_displayDerivative;
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
  Model m_model;
};

}

#endif
