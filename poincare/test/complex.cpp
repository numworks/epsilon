#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;
constexpr Expression::FloatDisplayMode DecimalDisplay = Expression::FloatDisplayMode::Decimal;
constexpr Expression::FloatDisplayMode ScientificDisplay = Expression::FloatDisplayMode::Scientific;
constexpr Expression::ComplexFormat Cartesian = Expression::ComplexFormat::Cartesian;
constexpr Expression::ComplexFormat Polar = Expression::ComplexFormat::Polar;

template<typename T>
void assert_cartesian_complex_prints_to(T a, T b, const char * result, Expression::FloatDisplayMode mode = ScientificDisplay, Expression::ComplexFormat format = Cartesian, int significantDigits = 7, int bufferSize = 13+13+7+1) {
  quiz_print(result);

  int tagSize = 8;
  unsigned char tag = 'X';
  char * taggedBuffer = new char[bufferSize+2*tagSize];
  memset(taggedBuffer, tag, bufferSize+2*tagSize);
  char * buffer = taggedBuffer + tagSize;

  if (b == 0) {
    Complex<T>::convertFloatToText(a, buffer, bufferSize, significantDigits, mode);
  } else {
    Preferences::sharedPreferences()->setComplexFormat(format);
    Preferences::sharedPreferences()->setDisplayMode(mode);
    Complex<T>::Cartesian(a, b).writeTextInBuffer(buffer, bufferSize);
  }

  for (int i=0; i<tagSize; i++) {
    assert(taggedBuffer[i] == tag);
  }
  for (int i=tagSize+strlen(buffer)+1; i<bufferSize+2*tagSize; i++) {
    assert(taggedBuffer[i] == tag);
  }

  for (int i=0; i<bufferSize; i++) {
    if (buffer[i] == Ion::Charset::Exponent) {
      buffer[i] = 'E';
    }
    if (buffer[i] == Ion::Charset::Exponential) {
      buffer[i] = 'e';
    }
    if (buffer[i] ==  Ion::Charset::IComplex) {
      buffer[i] = 'i';
    }
    if (buffer[i] ==  Ion::Charset::MiddleDot) {
      buffer[i] = '*';
    }
  }

  assert(strcmp(buffer, result) == 0);

  delete[] taggedBuffer;
}

QUIZ_CASE(poincare_complex_to_text) {
  /* We expect 7 significative numbers but do not display 0 */
  assert_cartesian_complex_prints_to(123.456f, 0.0f, "1.23456E2");
  assert_cartesian_complex_prints_to(1.234567891011, 0.0, "1.234568");
  assert_cartesian_complex_prints_to(2.0f, 0.0f, "2");
  assert_cartesian_complex_prints_to(123456789.0, 0.0, "1.234568E8");
  assert_cartesian_complex_prints_to(0.00000123456789f, 0.0f, "1.234568E-6");
  assert_cartesian_complex_prints_to(0.99, 0.0, "9.9E-1");
  assert_cartesian_complex_prints_to(-123.456789f, 0.0f, "-1.234568E2");
  assert_cartesian_complex_prints_to(-0.000123456789, 0.0, "-1.234568E-4");
  assert_cartesian_complex_prints_to(0.0f, 0.0f, "0");
  assert_cartesian_complex_prints_to(10000000000000000000000000000.0, 0.0, "1E28");
  /* Converting 10000000000000000000000000000.0f into a decimal display would
   * overflow the number of significant digits set to 7. When this is the case, the
   * display mode is automatically set to scientific. */
  assert_cartesian_complex_prints_to(10000000000000000000000000000.0, 0.0, "1E28", DecimalDisplay);
  assert_cartesian_complex_prints_to(1000000.0, 0.0, "1000000", DecimalDisplay);
  assert_cartesian_complex_prints_to(10000000.0f, 0.0f, "1E7", DecimalDisplay);
  assert_cartesian_complex_prints_to(0.000001, 0.0, "0.000001", DecimalDisplay);
  /* Converting 0.00000001f into a decimal display would also overflow the
   * number of significant digits set to 7. */
  assert_cartesian_complex_prints_to(0.0000001f, 0.0f, "1E-7", DecimalDisplay);
  assert_cartesian_complex_prints_to(-0.000000000000000000000000000000009090018, 0.0, "-9.090018E-33");
  assert_cartesian_complex_prints_to(123.421f, 0.0f, "123.4", DecimalDisplay, Cartesian, 4, 6);
  assert_cartesian_complex_prints_to(123.421, 0.0, "1.2E2", DecimalDisplay, Cartesian, 5, 6);
  assert_cartesian_complex_prints_to(9.999999f, 0.0f, "10", DecimalDisplay, Cartesian, 6);
  assert_cartesian_complex_prints_to(-9.99999904, 0.0, "-10", DecimalDisplay, Cartesian, 6);
}

QUIZ_CASE(poincare_complex_cartesian_to_text) {
  assert_cartesian_complex_prints_to(1.0, 2.0, "1+2*i", DecimalDisplay, Cartesian);
  assert_cartesian_complex_prints_to(1.0f, 2.0f, "2.236068*e^(1.107149*i)", DecimalDisplay, Polar);
  assert_cartesian_complex_prints_to(-1.3f, 2.444f, "-1.3+2.444*i", DecimalDisplay, Cartesian);
  assert_cartesian_complex_prints_to(-1.3, 2.444, "2.768237*e^(2.059649*i)", DecimalDisplay, Polar);
  assert_cartesian_complex_prints_to(-1.3f, -2.444f, "-1.3-2.444*i", DecimalDisplay, Cartesian);
  assert_cartesian_complex_prints_to(64078208.0, 119229408.0, "6.407821E7+1.192294E8*i", DecimalDisplay, Cartesian);
  assert_cartesian_complex_prints_to(64078208.0f, 119229408.0f, "1.353576E8*e^(1.07765*i)", DecimalDisplay, Polar);
  assert_cartesian_complex_prints_to(64078208.0f, 119229408.0f, "1.353576E8*e^(1.07765*i)", DecimalDisplay, Polar);
  assert_cartesian_complex_prints_to(INFINITY, 119229408.0f, "inf", DecimalDisplay, Polar);
  assert_cartesian_complex_prints_to(0.0f, 0.0f, "0", DecimalDisplay, Polar);
  assert_cartesian_complex_prints_to(NAN, 0.0f, "undef", DecimalDisplay, Polar);
  assert_cartesian_complex_prints_to(0.0f, NAN, "undef", DecimalDisplay, Polar);
  assert_cartesian_complex_prints_to(NAN, NAN, "undef", DecimalDisplay, Polar);
}

QUIZ_CASE(poincare_complex_evaluate) {
  GlobalContext globalContext;
  Expression * a = new Complex<float>(Complex<float>::Float(123.456f));
  Expression * m = a->approximate<double>(globalContext);
  assert(m->type() == Expression::Type::Complex);
  Complex<double> * mc = static_cast<Complex<double> *>(m);
  assert(std::fabs(mc->a() - 123.456) < 0.00001);
  assert(mc->b() == 0.0);
  delete m;

  Expression * n = a->approximate<float>(globalContext);
  assert(n->type() == Expression::Type::Complex);
  Complex<float> * nc = static_cast<Complex<float> *>(n);
  assert(nc->a() == 123.456f);
  assert(nc->b() == 0.0f);
  delete n;

  delete a;
}

QUIZ_CASE(poincare_complex_constructor) {
  Complex<float> * a = new Complex<float>(Complex<float>::Cartesian(2.0f, 3.0f));
  assert(std::fabs(a->a() - 2.0f) < 0.00001f && std::fabs(a->b()-3.0f) < 0.00001f);
  assert(a->r() == 3.60555124f && a->th() == 0.982793748f);
  delete a;

  a = new Complex<float>(Complex<float>::Polar(3.60555124f, 0.982793748f));
  assert(std::fabs(a->a() - 2.0f) < 0.00001f && std::fabs(a->b()-3.0f) < 0.00001f);
  delete a;

  Complex<double> * b = new Complex<double>(Complex<double>::Cartesian(1.0, 12.0));
  assert(std::fabs(b->a() - 1.0) < 0.0000000001 && std::fabs(b->b()-12.0) < 0.0000000001);
  delete b;

  b = new Complex<double>(Complex<double>::Polar(12.04159457879229548012824103, 1.4876550949));
  assert(std::fabs(b->a() - 1.0) < 0.0000000001 && std::fabs(b->b()-12.0) < 0.0000000001);
  delete b;

  Complex<float> * c = new Complex<float>(Complex<float>::Cartesian(-2.0e20f, 2.0e20f));
  assert(c->a() == -2.0e20f && c->b() == 2.0e20f);
  assert(c->r() == 2.0e20f*(float)M_SQRT2 && c->th() == 3*(float)M_PI_4);
  delete c;

  Complex<double> * d = new Complex<double>(Complex<double>::Cartesian(1.0e155, -1.0e155));
  assert(d->a() == 1.0e155 && d->b() == -1.0e155);
  assert(d->r() == 1.0e155*M_SQRT2 && d->th() == -M_PI_4);
  delete d;
}
