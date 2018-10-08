#ifndef GRAPH_STORAGE_CARTESIAN_FUNCTION_H
#define GRAPH_STORAGE_CARTESIAN_FUNCTION_H

#include "../shared/global_context.h"
#include "../shared/storage_function.h"
#include <poincare/symbol.h>

namespace Graph {

class StorageCartesianFunction : public Shared::StorageFunction {
public:
  static const char * Extension() { return Shared::GlobalContext::funcExtension; }
  static void DefaultName(char buffer[], size_t bufferSize);
  static StorageCartesianFunction NewModel(Ion::Storage::Record record);
  StorageCartesianFunction(const char * text = nullptr) :
    Shared::StorageFunction(text)
  {}
  StorageCartesianFunction(Ion::Storage::Record record) :
    Shared::StorageFunction(record)
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
  Poincare::Expression::Coordinate2D nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const Shared::StorageFunction * function) const;
  const char * symbol() const override {
    static const char x[2] = {Poincare::Symbol::UnknownX, 0}; //TODO remove static
    return x;
  }
  // StorageExpressionModel
  void setContent(const char * c) override;
};

}

#endif
