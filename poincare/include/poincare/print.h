#ifndef POINCARE_PRINT_H
#define POINCARE_PRINT_H

#include <escher/metric.h>
#include <ion/display_constants.h>
#include <kandinsky/font.h>
#include <stdarg.h>
#include <stddef.h>

namespace Escher {
class AbstractBufferTextView;
}
namespace Poincare {
class Print {
  friend class Escher::AbstractBufferTextView;

 public:
  /* Custom printf which supports the following formats:
   * - %c for character insertion
   * - %s for string insertion
   * - %Cs for string insertion capitalizing the first character
   * - %cs for string insertion decapitalizing the first character
   *   ex:
   *     CustomPrintf("A capitalized string: %Cs", "plouf")
   *       --> "A capitalized string: Plouf"
   *     CustomPrintf("A decapitalized string: %cs", "Plouf")
   *       --> "A decapitalized string: plouf"
   * - %i for integer insertion
   * - %*.*ef or %*.*ed for respectively float or double insertion.
   *   ex:
   *     CustomPrintf("A float %*.*ef!",
   *                  0.0123456789f,
   *                  Preferences::PrintFloatMode::Decimal,
   *                  7)
   *       --> "A float 0.01234568!"
   *     CustomPrintf("A double %*.*ed!",
   *                   0.0123456789,
   *                   Preferences::PrintFloatMode::Scientific,
   *                   4)
   *       --> "A double 1.235E-2!"
   * - %+i or %+*.*ef inserts the plus sign if the value is positive or null
   *
   * In contrast to UnsafeCustomPrintf, CustomPrintf method asserts that the
   * bufferSize is large enough. If the text does not fit, the buffer is erased
   * and a length equal or bigger than bufferSize is returned.
   */
  static int CustomPrintf(char* buffer, size_t bufferSize, const char* format,
                          ...);
  static int UnsafeCustomPrintf(char* buffer, size_t bufferSize,
                                const char* format, ...);

  /* Try to print in buffer, and each time the text is too long, decrease by one
   * the number of significant digits, until it fits.
   * For this method, do not pass a number of significant digits as argument to
   * each printed float/double.
   * ex:
   *   CustomPrintfWithMaxNumberOfGlyphs(
   *                 buffer, bufferSize, 7, maxNumberOfGlyphs
   *                 "Two doubles are %*.*ed and %*.*ed",
   *                 0.0123456789,
   *                 Preferences::PrintFloatMode::Scientific,
   *                 9.8765432109,
   *                 Preferences::PrintFloatMode::Scientific);
   * */
  constexpr static int k_maxNumberOfSmallGlyphsInScreenWidth =
      (Ion::Display::Width - 2 * Escher::Metric::CommonLargeMargin) /
      KDFont::GlyphWidth(KDFont::Size::Small);
  static int CustomPrintfWithMaxNumberOfGlyphs(char* buffer, size_t bufferSize,
                                               int maxNumberOfSignificantDigits,
                                               int maxNumberOfGlyphs,
                                               const char* format, ...);

  static void Capitalize(char* text);
  static void Decapitalize(char* text);

 private:
  static int PrivateCustomPrintf(char* buffer, size_t bufferSize,
                                 const char* format, va_list args) {
    return PrivateCustomPrintf(buffer, bufferSize, -1, format, args);
  }
  // If numberOfSignificantDigits < 0, use numberOfSignificantDigits in va_list
  static int PrivateCustomPrintf(char* buffer, size_t bufferSize,
                                 int numberOfSignificantDigits,
                                 const char* format, va_list args);
};

}  // namespace Poincare

#endif
