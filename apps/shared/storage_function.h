#ifndef SHARED_STORAGE_FUNCTION_H
#define SHARED_STORAGE_FUNCTION_H

#include <poincare/function.h>
#include "storage_expression_model.h"

namespace Shared {

class StorageFunction : public StorageExpressionModel {
public:
  enum class NameNotCompliantError {
    None = 0,
    CharacterNotAllowed,
    NameCannotStartWithNumber,
    ReservedName
  };
  constexpr static int k_parenthesedArgumentLength = 3;
  static constexpr char k_parenthesedArgument[k_parenthesedArgumentLength+1] = "(x)";
  constexpr static int k_maxNameWithArgumentSize = Poincare::SymbolAbstract::k_maxNameSize + k_parenthesedArgumentLength; /* Function name and null-terminating char + "(x)" */;
  static bool BaseNameCompliant(const char * baseName, NameNotCompliantError * error = nullptr);

  // Constructors
  StorageFunction(Ion::Storage::Record record) : StorageExpressionModel(record){}

  // Properties
  bool isActive() const;
  KDColor color() const;
  void setActive(bool active);

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
    FunctionRecordData(KDColor color) : m_color(color), m_active(true) {}
    KDColor color() const {
      /* Record::value() is a pointer to an address inside
       * Ion::Storage::sharedStorage(), and it might be unaligned. In the method
       * recordData(), we cast Record::value() to the type FunctionRecordData.
       * We must thus do some convolutions to read KDColor, which is a uint16_t
       * and might produce an alignment error on the emscripten platform. */
      return KDColor::RGB16(Ion::StorageHelper::unalignedShort(reinterpret_cast<char *>(const_cast<KDColor *>(&m_color))));
    }
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
