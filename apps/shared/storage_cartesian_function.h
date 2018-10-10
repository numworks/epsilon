#ifndef SHARED_STORAGE_CARTESIAN_FUNCTION_H
#define SHARED_STORAGE_CARTESIAN_FUNCTION_H

#include "global_context.h"
#include "storage_function.h"
#include <poincare/symbol.h>

namespace Shared {

class StorageCartesianFunction : public StorageFunction {
public:
  static void DefaultName(char buffer[], size_t bufferSize);
  static StorageCartesianFunction NewModel(Ion::Storage::Record::ErrorStatus * error);
  StorageCartesianFunction(Ion::Storage::Record record = Record()) :
    StorageFunction(record)
  {}
  //bool operator==(const StorageCartesianFunction & other) const { return *(static_cast<Record *>(this)) == static_cast<Record &>(other)); }
  //bool operator!=(const StorageCartesianFunction & other) const { return !(*(static_cast<Record *>(this)) == static_cast<Record &>(other)); }
  bool displayDerivative() const;
  void setDisplayDerivative(bool display);
  double approximateDerivative(double x, Poincare::Context * context) const;
  double sumBetweenBounds(double start, double end, Poincare::Context * context) const override;
  Poincare::Expression::Coordinate2D nextMinimumFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Expression::Coordinate2D nextMaximumFrom(double start, double step, double max, Poincare::Context * context) const;
  double nextRootFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Expression::Coordinate2D nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const StorageFunction * function) const;
protected:
  class CartesianFunctionRecordData : public FunctionRecordData {
  public:
    CartesianFunctionRecordData() :
      FunctionRecordData(),
    m_displayDerivative(true)
    {}
    bool displayDerivative() const { return m_displayDerivative; }
    void setDisplayDerivative(bool display) { m_displayDerivative = display; }
  private:
    bool m_displayDerivative;
    //char m_expression[0];
  };
private:
  size_t metaDataSize() const override { return sizeof(CartesianFunctionRecordData); }
  CartesianFunctionRecordData * recordData() const;
};

}

#endif
