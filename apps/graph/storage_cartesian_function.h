#ifndef GRAPH_STORAGE_CARTESIAN_FUNCTION_H
#define GRAPH_STORAGE_CARTESIAN_FUNCTION_H

#include "../shared/storage_function.h"
#include <poincare/global_context.h>

namespace Graph {

class StorageCartesianFunction : public Shared::StorageFunction {
public:
  static const char * Extension() { return Poincare::GlobalContext::funcExtension; }
  static StorageCartesianFunction EmptyModel() { return StorageCartesianFunction("function", KDColorRed); }
  StorageCartesianFunction(const char * text = nullptr, KDColor color = KDColorBlack);
  StorageCartesianFunction(Ion::Storage::Record record);
  bool operator==(const StorageCartesianFunction & other) const { return m_record == other.m_record; }
  bool operator!=(const StorageCartesianFunction & other) const { return !(*this == other); }
  bool displayDerivative() const { return m_displayDerivative; }
  void setDisplayDerivative(bool display) { m_displayDerivative = display; }
  double approximateDerivative(double x, Poincare::Context * context) const;
  double sumBetweenBounds(double start, double end, Poincare::Context * context) const override;
  Poincare::Expression::Coordinate2D nextMinimumFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Expression::Coordinate2D nextMaximumFrom(double start, double step, double max, Poincare::Context * context) const;
  double nextRootFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Expression::Coordinate2D nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const Shared::StorageFunction * function) const;
  const char * symbol() const override { return "x"; }
private:
  bool m_displayDerivative;
};

}

#endif
