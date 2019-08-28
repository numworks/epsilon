#ifndef SHARED_FUNCTION_H
#define SHARED_FUNCTION_H

#include <poincare/function.h>
#include <poincare/symbol.h>
#include "expression_model_handle.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Shared {

class Function : public ExpressionModelHandle {
public:
  enum class NameNotCompliantError {
    None = 0,
    CharacterNotAllowed,
    NameCannotStartWithNumber,
    ReservedName
  };
  /* Possible arguments: n, x, t, θ
   * The CodePoint θ is two char long. */
  constexpr static int k_parenthesedArgumentLength = 4;
  constexpr static int k_maxNameWithArgumentSize = Poincare::SymbolAbstract::k_maxNameSize + k_parenthesedArgumentLength; /* Function name and null-terminating char + "(x)" */;
  static bool BaseNameCompliant(const char * baseName, NameNotCompliantError * error = nullptr);

  // Constructors
  Function(Ion::Storage::Record record) : ExpressionModelHandle(record){}

  // Properties
  bool isActive() const;
  KDColor color() const;
  void setActive(bool active);

  // Definition Interval
  virtual double tMin() const { return NAN; }
  virtual double tMax() const { return NAN; }

  // Name
  int nameWithArgument(char * buffer, size_t bufferSize);

  // Evaluation
  virtual Poincare::Coordinate2D<float> evaluate2DAtParameter(float t, Poincare::Context * context) const = 0;
  virtual Poincare::Coordinate2D<double> evaluate2DAtParameter(double t, Poincare::Context * context) const = 0;
protected:
  /* FunctionRecordDataBuffer is the layout of the data buffer of Record
   * representing a Function. We want to avoid padding which would:
   * - increase the size of the storage file
   * - introduce junk memory zone which are then crc-ed in Storage::checksum
   *   creating dependency on uninitialized values. */
#pragma pack(push,1)
  class FunctionRecordDataBuffer {
  public:
    FunctionRecordDataBuffer(KDColor color) : m_color(color), m_active(true) {}
    KDColor color() const {
      return KDColor::RGB16(m_color);
    }
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
  private:
#if __EMSCRIPTEN__
    /* Record::value() is a pointer to an address inside
     * Ion::Storage::sharedStorage(), and it might be unaligned. However, for
     * emscripten memory representation, loads and stores must be aligned;
     * performing a normal load or store on an unaligned address can fail
     * silently. We thus use 'emscripten_align1_short' type, the unaligned
     * version of uint16_t type to avoid producing an alignment error on the
     * emscripten platform. */
    static_assert(sizeof(emscripten_align1_short) == sizeof(uint16_t), "emscripten_align1_short should have the same size as uint16_t");
    emscripten_align1_short m_color;
#else
    uint16_t m_color;
#endif
    bool m_active;
  };
#pragma pack(pop)
private:
  FunctionRecordDataBuffer * recordData() const;
};

}

#endif
