#ifndef SHARED_STORAGE_FUNCTION_H
#define SHARED_STORAGE_FUNCTION_H

#include <escher/text_field.h>
#include <poincare/function.h>
#include "storage_expression_model.h"

namespace Shared {

class StorageFunction : public StorageExpressionModel {
public:
  // Constructors
  StorageFunction(const char * name = nullptr) : StorageExpressionModel(name) {}
  StorageFunction(Ion::Storage::Record record) : StorageExpressionModel(record){}

  // Checksum
  virtual uint32_t checksum();

  // Properties
  bool isActive() const;
  KDColor color() const;
  void setActive(bool active);
  void setColor(KDColor color);

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
  constexpr static size_t k_dataLengthInBytes = (TextField::maxBufferSize()+2)*sizeof(char)+2;
  static_assert((k_dataLengthInBytes & 0x3) == 0, "The function data size is not a multiple of 4 bytes (cannot compute crc)"); // Assert that dataLengthInBytes is a multiple of 4
  template<typename T> T templatedApproximateAtAbscissa(T x, Poincare::Context * context) const;
  FunctionRecordData * recordData() const;
  virtual const char * symbol() const = 0;
};

}

#endif
