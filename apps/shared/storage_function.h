#ifndef SHARED_STORAGE_FUNCTION_H
#define SHARED_STORAGE_FUNCTION_H

#include <escher/text_field.h>
#include <poincare/function.h>
#include "storage_expression_model.h"

namespace Shared {

class StorageFunction : public StorageExpressionModel {
public:
  // Constructors
  StorageFunction(Ion::Storage::Record record) : StorageExpressionModel(record){}

  // Checksum
  virtual uint32_t checksum();

  // Properties
  bool isActive() const;
  KDColor color() const;
  void setActive(bool active);
  void setColor(KDColor color);

  // Name
  int nameWithArgument(char * buffer, size_t bufferSize, char arg);

  // Evaluation
  virtual float evaluateAtAbscissa(float x, Poincare::Context * context) const {
    return templatedApproximateAtAbscissa(x, context);
  }
  virtual double evaluateAtAbscissa(double x, Poincare::Context * context) const {
    return templatedApproximateAtAbscissa(x, context);
  }
  virtual double sumBetweenBounds(double start, double end, Poincare::Context * context) const = 0;
protected:
  class FunctionRecordData {
  public:
    FunctionRecordData() : m_color(KDColorRed), m_active(true) {}
    KDColor color() const { return m_color; }
    void setColor(KDColor color) { m_color = color; }
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
  private:
    KDColor m_color;
    bool m_active;
  };
private:
  template<typename T> T templatedApproximateAtAbscissa(T x, Poincare::Context * context) const;
  FunctionRecordData * recordData() const;
};

}

#endif
