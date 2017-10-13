#include <poincare/trigonometry.h>
#include <ion.h>

namespace Poincare {

static_assert('\x89' == Ion::Charset::SmallPi, "Incorrect");
constexpr const char * cheatTable[Trigonometry::k_numberOfEntries][3] = {{"\x89*12^(-1)", "\x89*5*12^(-1)", "6^(1/2)*4^(-1)+2^(1/2)*4^(-1)"}, {"\x89*8^(-1)", "\x89*3*8^(-1)", "(2+2^(1/2))^(1/2)*2^(-1)"}, {"\x89*6^(-1)", "\x89*3^(-1)", "3^(1/2)*2^(-1)"}};

Expression * Trigonometry::table(const Expression * e, Function f, bool inverse) {
  int inputIndex = inverse ? 2 : (int)f;
  int outputIndex = inverse ? (int)f : 2;
  for (int i = 0; i < k_numberOfEntries; i++) {
    Expression * input = Expression::parse(cheatTable[i][inputIndex]);
    input->simplify(); // input expression does not change, no root needed and we can use entry after
    if (input->compareTo(e) == 0) {
      Expression * output = Expression::parse(cheatTable[i][outputIndex]);
      return output->simplify();
    }
  }
  return nullptr;
}

}
