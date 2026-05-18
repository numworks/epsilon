#pragma once

#include <escher/theme/components.h>
#include <poincare/helpers/symbol.h>

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
      Poincare::SymbolHelper::k_maxNameSize +
      k_parenthesedThetaArgumentByteLength; /* Function name and
                                               null-terminating char + "(θ)" */
  ;

  constexpr static char k_unknownName[2] = {UCodePointUnknown, 0};
  static size_t WithArgument(CodePoint argument, char* buffer,
                             size_t bufferSize);
  static size_t NameWithArgument(Ion::Storage::Record record,
                                 CodePoint argument, char* buffer,
                                 size_t bufferSize);

  // Constructors
  Function(Ion::Storage::Record record) : ExpressionModelHandle(record) {}

  // Properties
  virtual bool isActive() const { return recordData()->isActive(); }
  Escher::ThemeComponent color() const { return recordData()->color(); }
  void setColor(Escher::ThemeComponent color) { recordData()->setColor(color); }
  void setActive(bool active);
  virtual int numberOfSubCurves() const { return 1; }
  virtual bool isAlongY() const { return false; }

  // Definition Interval
  virtual float tMin() const { return NAN; }
  virtual float tMax() const { return NAN; }
  virtual float rangeStep() const { return NAN; }

  // Name
  size_t name(char* buffer, size_t bufferSize) const;
  size_t withArgument(char* buffer, size_t bufferSize) const;
  virtual size_t nameWithArgument(char* buffer, size_t bufferSize,
                                  int derivationOrder = 0) const;
  virtual size_t printAbscissaValue(double cursorT, double cursorX,
                                    char* buffer, size_t bufferSize,
                                    int precision) const;
  // Insert the value of the evaluation in buffer
  virtual size_t printFunctionValue(double cursorT, double cursorX,
                                    double cursorY, char* buffer,
                                    size_t bufferSize, int precision) const;

  // Evaluation
  virtual Poincare::Coordinate2D<float> evaluateXYAtParameter(
      float t, int subCurveIndex = 0) const = 0;
  virtual Poincare::Coordinate2D<double> evaluateXYAtParameter(
      double t, int subCurveIndex = 0) const = 0;
  virtual Poincare::SystemExpression sumBetweenBounds(double start,
                                                      double end) const = 0;

 protected:
  /* RecordDataBuffer is the layout of the data buffer of Record
   * representing a Function. We want to avoid padding which would:
   * - increase the size of the storage file
   * - introduce junk memory zone which are then crc-ed in Storage::checksum
   *   creating dependency on uninitialized values.
   * - complicate getters, setters and record handling
   * In addition, Record::value() is a pointer to an address inside
   * Ion::Storage::FileSystem::sharedFileSystem, and it might be unaligned. We
   * use the packed keyword to warn the compiler that its members are
   * potentially unaligned (otherwise, the compiler can emit instructions that
   * work only on aligned objects). It also solves the padding issue mentioned
   * above.
   */
  class __attribute__((packed)) RecordDataBuffer {
   public:
    RecordDataBuffer(Escher::ThemeComponent color)
        : m_color(color), m_active(true) {}
    Escher::ThemeComponent color() const { return m_color; }
    void setColor(Escher::ThemeComponent color) { m_color = color; }
    virtual bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

   private:
    Escher::ThemeComponent m_color;
    bool m_active;
  };

  virtual void didBecomeInactive() {}

 private:
  RecordDataBuffer* recordData() const;
};

}  // namespace Shared
