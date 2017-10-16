#include <poincare/trigonometry.h>
#include <poincare/simplification_root.h>
#include <ion.h>

namespace Poincare {

static_assert('\x89' == Ion::Charset::SmallPi, "Incorrect");
constexpr const char * cheatTable[Trigonometry::k_numberOfEntries][3] =
{{"\x89",            "\x89*(-2)^(-1)",       "-1"},
 {"\x89*11*12^(-1)", "\x89*(-5)*12^(-1)", "(-1)*6^(1/2)*4^(-1)-2^(1/2)*4^(-1)"},
 {"\x89*7*8^(-1)",   "\x89*(-3)*8^(-1)",  "-(2+2^(1/2))^(1/2)*2^(-1)"},
 {"\x89*5*6^(-1)",   "\x89*(-3)^(-1)",    "-3^(1/2)*2^(-1)"},
 {"\x89*3*4^(-1)",   "\x89*(-4)^(-1)",    "(-1)*(2^(-1/2))"},
 {"\x89*2*3^(-1)",   "\x89*(-6)^(-1)",    "-0.5" },
 {"\x89*5*8^(-1)",   "\x89*(-8)^(-1)",    "(2-2^(1/2))^(1/2)*(-2)^(-1)"},
 {"\x89*7*12^(-1)",  "\x89*(-12)^(-1)",   "-6^(1/2)*4^(-1)+2^(1/2)*4^(-1)"},
 {"\x89*2^(-1)",     "0",                 "0"},
 {"\x89*2^(-1)",     "\x89",              "0"},
 {"\x89*5*12^(-1)",  "\x89*12^(-1)",      "6^(1/2)*4^(-1)+2^(1/2)*(-4)^(-1)"},
 {"\x89*3^(-1)",     "\x89*6^(-1)",       "0.5"},
 {"\x89*3*8^(-1)",   "\x89*8^(-1)",       "(2-2^(1/2))^(1/2)*2^(-1)"},
 {"\x89*4^(-1)",     "\x89*4^(-1)",       "2^(-1/2)"},
 {"\x89*6^(-1)",     "\x89*3^(-1)",       "3^(1/2)*2^(-1)"},
 {"\x89*8^(-1)",     "\x89*3*8^(-1)",     "(2+2^(1/2))^(1/2)*2^(-1)"},
 {"\x89*12^(-1)",    "\x89*5*12^(-1)",    "6^(1/2)*4^(-1)+2^(1/2)*4^(-1)"},
 {"0",               "\x89*2^(-1)",    "1"}
};

Expression * Trigonometry::table(const Expression * e, Function f, bool inverse) {
  int inputIndex = inverse ? 2 : (int)f;
  int outputIndex = inverse ? (int)f : 2;
  for (int i = 0; i < k_numberOfEntries; i++) {
    Expression * input = Expression::parse(cheatTable[i][inputIndex]);
    SimplificationRoot inputRoot(input);
    inputRoot.simplify(); // input expression does not change, no root needed and we can use entry after
    if (inputRoot.operand(0)->compareTo(e) == 0) {
      Expression * output = Expression::parse(cheatTable[i][outputIndex]);
      SimplificationRoot outputRoot(output);
      return (Expression *)(outputRoot.simplify())->operand(0);
    }
  }
  return nullptr;
}

}
