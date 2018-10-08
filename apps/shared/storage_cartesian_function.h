#ifndef SHARED_STORAGE_CARTESIAN_FUNCTION_H
#define SHARED_STORAGE_CARTESIAN_FUNCTION_H

#include "global_context.h"
#include "storage_function.h"
#include <poincare/symbol.h>

namespace Shared {

class StorageCartesianFunction : public StorageFunction {
public:
  static const char * Extension() { return GlobalContext::funcExtension; }
  static void DefaultName(char buffer[], size_t bufferSize);
  static StorageCartesianFunction NewModel();
  StorageCartesianFunction(const char * text = nullptr) :
    StorageFunction(text)
  {}
  StorageCartesianFunction(Ion::Storage::Record record) :
    StorageFunction(record)
  {}
  bool operator==(const StorageCartesianFunction & other) const { return record() == other.record(); }
  bool operator!=(const StorageCartesianFunction & other) const { return !(*this == other); }
  bool displayDerivative() const;
  void setDisplayDerivative(bool display);
  double approximateDerivative(double x, Poincare::Context * context) const;
  double sumBetweenBounds(double start, double end, Poincare::Context * context) const override;
  Poincare::Expression::Coordinate2D nextMinimumFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Expression::Coordinate2D nextMaximumFrom(double start, double step, double max, Poincare::Context * context) const;
  double nextRootFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Expression::Coordinate2D nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const StorageFunction * function) const;
  const char * symbol() const override {
    static const char x[2] = {Poincare::Symbol::UnknownX, 0}; //TODO remove static
    return x;
  }
  // StorageExpressionModel
  void setContent(const char * c) override;
  void * expressionAddress() const override;
  size_t expressionSize() const override;
protected:
  class CartesianFunctionRecordData : public FunctionRecordData {
  public:
    CartesianFunctionRecordData() :
      FunctionRecordData(),
      m_displayDerivative(true)
    {}
    bool displayDerivative() const { return m_displayDerivative; }
    void setDisplayDerivative(bool display) { m_displayDerivative = display; }
    void * expressionAddress() { return &m_expression; }
  private:
    bool m_displayDerivative;
    char m_expression[0];
  };
private:
  CartesianFunctionRecordData * recordData() const;
};

}

#endif
