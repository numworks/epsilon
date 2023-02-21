#ifndef SHARED_FUNCTION_H
#define SHARED_FUNCTION_H

#include <escher/i18n.h>
#include <poincare/function.h>
#include <poincare/symbol.h>

#include "expression_model_handle.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Shared {

class Function : public ExpressionModelHandle {
 public:
  /* Possible arguments: n, x, t, θ
   * The CodePoint θ is two char long. */
  constexpr static int k_parenthesedArgumentCodePointLength = 3;
  constexpr static int k_parenthesedThetaArgumentByteLength = 4;
  constexpr static int k_parenthesedXNTArgumentByteLength = 3;
  constexpr static int k_maxNameWithArgumentSize =
      Poincare::SymbolAbstract::k_maxNameSize +
      k_parenthesedThetaArgumentByteLength; /* Function name and
                                               null-terminating char + "(θ)" */
  ;

  constexpr static char k_unknownName[2] = {UCodePointUnknown, 0};
  // Constructors
  Function(Ion::Storage::Record record) : ExpressionModelHandle(record) {}

  // Properties
  virtual bool isActive() const;
  KDColor color() const;
  void setColor(KDColor color);
  void setActive(bool active);
  virtual int numberOfSubCurves() const { return 1; }
  virtual bool isAlongY() const { return false; }

  // Definition Interval
  virtual float tMin() const { return NAN; }
  virtual float tMax() const { return NAN; }
  virtual float rangeStep() const { return NAN; }

  // Name
  int name(char* buffer, size_t bufferSize);
  virtual int nameWithArgument(char* buffer, size_t bufferSize);
  // Insert the value of the evaluation (or the symbol if symbolValue) in buffer
  virtual int printValue(double cursorT, double cursorX, double cursorY,
                         char* buffer, int bufferSize, int precision,
                         Poincare::Context* context, bool symbolValue = false);
  virtual I18n::Message parameterMessageName() const = 0;

  // Evaluation
  virtual Poincare::Coordinate2D<float> evaluateXYAtParameter(
      float t, Poincare::Context* context, int subCurveIndex = 0) const = 0;
  virtual Poincare::Coordinate2D<double> evaluateXYAtParameter(
      double t, Poincare::Context* context, int subCurveIndex = 0) const = 0;
  virtual Poincare::Expression sumBetweenBounds(
      double start, double end, Poincare::Context* context) const = 0;

  // Range
  virtual bool basedOnCostlyAlgorithms(Poincare::Context* context) const = 0;

 protected:
  /* RecordDataBuffer is the layout of the data buffer of Record
   * representing a Function. We want to avoid padding which would:
   * - increase the size of the storage file
   * - introduce junk memory zone which are then crc-ed in Storage::checksum
   *   creating dependency on uninitialized values.
   * - complicate getters, setters and record handling
   * In addition, Record::value() is a pointer to an address inside
   * Ion::Storage::FileSystem::sharedFileSystem, and it might be unaligned. We
   * use the packed keyword to warn the compiler that it members are potentially
   * unaligned (otherwise, the compiler can emit instructions that work only on
   * aligned objects). It also solves the padding issue mentioned above.
   */
  class __attribute__((packed)) RecordDataBuffer {
   public:
    RecordDataBuffer(KDColor color) : m_color(color), m_active(true) {}
    KDColor color() const { return KDColor::RGB16(m_color); }
    void setColor(KDColor color) { m_color = color; }
    virtual bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

   private:
#if __EMSCRIPTEN__
    /* For emscripten memory representation, loads and stores must be aligned;
     * performing a normal load or store on an unaligned address can fail
     * silently. We thus use 'emscripten_align1_short' type, the unaligned
     * version of uint16_t type to avoid producing an alignment error on the
     * emscripten platform. */
    static_assert(
        sizeof(emscripten_align1_short) == sizeof(uint16_t),
        "emscripten_align1_short should have the same size as uint16_t");
    emscripten_align1_short m_color;
#else
    uint16_t m_color;
#endif
    bool m_active;
  };

  virtual void didBecomeInactive() {}

 private:
  RecordDataBuffer* recordData() const;
};

}  // namespace Shared

#endif
