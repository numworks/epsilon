#ifndef SHARED_STORAGE_CARTESIAN_FUNCTION_H
#define SHARED_STORAGE_CARTESIAN_FUNCTION_H

#include "global_context.h"
#include "storage_function.h"
#include <poincare/symbol.h>

namespace Shared {

class StorageCartesianFunction : public StorageFunction {
public:
  static void DefaultName(char buffer[], size_t bufferSize);
  static char Symbol() { return 'x'; }
  char symbol() const override { return Symbol(); };
  static StorageCartesianFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);
  StorageCartesianFunction(Ion::Storage::Record record = Record()) :
    StorageFunction(record)
  {}

  // Derivative
  bool displayDerivative() const;
  void setDisplayDerivative(bool display);
  int derivativeNameWithArgument(char * buffer, size_t bufferSize, char arg);
  double approximateDerivative(double x, Poincare::Context * context) const;
  // Integral
  double sumBetweenBounds(double start, double end, Poincare::Context * context) const override;
  // Extremum
  Poincare::Expression::Coordinate2D nextMinimumFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Expression::Coordinate2D nextMaximumFrom(double start, double step, double max, Poincare::Context * context) const;
  // Roots
  double nextRootFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Expression::Coordinate2D nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, Poincare::Expression expression) const;
private:
  class CartesianFunctionRecordData : public FunctionRecordData {
  public:
    CartesianFunctionRecordData(KDColor color) :
      FunctionRecordData(color),
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
  class Handle : public ExpressionModelHandle {
  public:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
  private:
    size_t expressionSize(const Ion::Storage::Record * record) const override;
  };
  size_t metaDataSize() const override { return sizeof(CartesianFunctionRecordData); }
  const ExpressionModelHandle * handle() const override { return &m_handle; }
  CartesianFunctionRecordData * recordData() const;
  Handle m_handle;
};

}

#endif
